//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_calib.hpp>
#include <linescan/exception_catcher.hpp>
#include <linescan/calc_laser_line.hpp>
#include <linescan/to_image.hpp>
#include <linescan/load.hpp>
#include <linescan/save.hpp>
#include <linescan/draw.hpp>

#include <QtCore/QSettings>

#include <fstream>
#include <iomanip>


namespace linescan{


	widget_calib::widget_calib(camera& cam, control_F9S_MCL3& mcl3):
		widget_processing_base(cam),
		cam_(cam),
		mcl3_(mcl3),
		laser_line_(tr("Laser image")),
		laser_auto_stop_l_(tr("Auto stop"))
	{
		radio_buttons_.addButton(&laser_line_);

		glayout_.addWidget(&laser_line_, 5, 0, 1, 2);
		glayout_.addWidget(&step_l_, 6, 0, 1, 2);
		glayout_.addWidget(&laser_start_, 7, 0, 1, 2);
		glayout_.addWidget(&laser_auto_stop_l_, 8, 0, 1, 1);
		glayout_.addWidget(&laser_auto_stop_, 8, 1, 1, 1);
		glayout_.setRowStretch(9, 1);

		connect(&laser_line_, &QRadioButton::released, [this]{
			if(!laser_line_.isChecked()) return;

			if(step_ != step::calib_yz){
				image_.set_processor(
					[this](mitrax::raw_bitmap< std::uint8_t >&& bitmap){
						auto overlay = draw_laser_alignment(
							bitmap, get_threashold(), get_erode()
						);

						return std::pair< QImage, QImage >(
							to_image(std::move(bitmap)), overlay
						);
					});
			}else{
				image_.set_processor(
					[this](mitrax::raw_bitmap< std::uint8_t >&& bitmap){
						auto overlay = draw_circle_line(bitmap);

						return std::pair< QImage, QImage >(
							to_image(std::move(bitmap)), overlay
						);
					});
			}
		});

		auto start_live = [this]{ image_.start_live(); };
		connect(&original_, &QRadioButton::released, start_live);
		connect(&binarized_, &QRadioButton::released, start_live);
		connect(&eroded_, &QRadioButton::released, start_live);
		connect(&laser_line_, &QRadioButton::released, start_live);

		connect(&laser_start_, &QPushButton::released, [this]{
			if(running_){
				switch(step_){
					case step::align: align_ready(); break;
					case step::calib_yz: analyze_yz(); break;
					case step::calib_x: analyze_x(); break;
					case step::complete: reset(); break;
				}
			}else{
				start();
			}
		});

		connect(&timer_, &QTimer::timeout, [this]{
			bool exception = false;

			if(step_ == step::calib_yz) exception |= !exception_catcher([&]{
				auto name = QString("data/calib_yz/laser_%1.png")
					.arg(save_count_line_, 4, 10, QLatin1Char('0'))
					.toStdString();

				++save_count_line_;

#ifdef CAM
				auto image = cam_.image();
				(void)name;
// 				save(image, name);
#else
				auto image = load(name);
#endif

				bitmap_ = mitrax::transform([](auto a, auto b){
					return std::min(a, b);
				}, bitmap_, image);

				auto c = [this, &image](){
					if(circle_calib_.size() < 4){
						return find_calib_circles(image);
					}

					std::vector< mitrax::point< double > > x1;
					std::vector< mitrax::point< double > > y1;
					std::vector< mitrax::point< double > > x2;
					std::vector< mitrax::point< double > > y2;
					for(auto const& v: circle_calib_){
						x1.emplace_back(v.Z, v.c1.x());
						y1.emplace_back(v.Z, v.c1.y());
						x2.emplace_back(v.Z, v.c2.x());
						y2.emplace_back(v.Z, v.c2.y());
					}

					return find_calib_circles(
						image,
						circle(
							fit_polynom< 3 >(x1)(mcl3_.read_z()),
							fit_polynom< 3 >(y1)(mcl3_.read_z()),
							circle_calib_.back().c1.radius()
						),
						circle(
							fit_polynom< 3 >(x2)(mcl3_.read_z()),
							fit_polynom< 3 >(y2)(mcl3_.read_z()),
							circle_calib_.back().c2.radius()
						)
					);
				}();

				image_.set_images(
					to_image(bitmap_),
					draw_circle_line(image, c)
				);

				if(c.size() != 2 || c[0].x() == c[1].x()){
					return;
				}

				if(!circle_calib_.empty() && (
					c[0].radius() < circle_calib_.back().c1.radius() * 0.8 ||
					c[1].radius() < circle_calib_.back().c2.radius() * 0.8 ||
					c[0].radius() > circle_calib_.back().c1.radius() * 1.2 ||
					c[1].radius() > circle_calib_.back().c2.radius() * 1.2
				)){
					return;
				}

				auto line = to_polynom(to_point(c[0]), to_point(c[1]));
				circle_calib_.emplace_back(
					c[0], c[1], line(cam_.cols() / 2), mcl3_.read_z()
				);
			}, false);

			exception |= !exception_catcher([&]{
				mcl3_.move_relative(0, 0, 1000);
			}, false);

			if(
				step_ == step::calib_yz &&
				circle_calib_.size() > 1 && laser_auto_stop_.isChecked()
			){
				auto y1 = cam_.rows() - circle_calib_.front().y;
				auto y2 = circle_calib_.back().y;

				if(y2 <= y1) analyze_yz();
			}

			if(exception){
				++exception_count_;
			}else{
				exception_count_ = 0;
			}

			if(exception_count_ > 9){
				QMessageBox box(
					QMessageBox::Warning,
					QObject::tr("Error"),
					tr("%1 errors back-to-back, cancle calibration?")
						.arg(exception_count_),
					QMessageBox::Yes | QMessageBox::No
				);

				exception_count_ = 0;

				if(box.exec() == QMessageBox::Yes) reset();
			}

			if(running_) timer_.start(1);
		});

		step_l_.setAlignment(Qt::AlignCenter);
		laser_auto_stop_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		laser_auto_stop_l_.setEnabled(false);
		laser_auto_stop_.setEnabled(false);

		set_step(step::calib_yz);
	}

	bool widget_calib::is_running()const{
		return running_;
	}

	void widget_calib::stop(){
		set_running(false);
	}

	void widget_calib::start(){
		set_running(true);
	}

	void widget_calib::hideEvent(QHideEvent* event){
		QWidget::hideEvent(event);
		stop();
	}

	QImage widget_calib::draw_yz(
		std::vector< mitrax::point< double > > left_points,
		std::vector< mitrax::point< double > > right_points,
		std::vector< mitrax::point< double > > y_to_Z_points,
		double height_min, double height_max
	)const{
		auto left_line = fit_polynom< 1 >(left_points);
		auto right_line = fit_polynom< 1 >(right_points);
		auto y_to_Z = fit_polynom< 3 >(y_to_Z_points);

		// Create a transparent Overlay
		QImage overlay(bitmap_.cols(), bitmap_.rows(), QImage::Format_ARGB32);
		overlay.fill(0);

		QPainter painter(&overlay);
		painter.setRenderHint(QPainter::Antialiasing, true);

		// Draw mm per pixel functions
		painter.setPen(QPen(QBrush(qRgb(255, 0, 0)), 2));
		painter.drawLine(
			left_line(0), 0,
			left_line(overlay.height() - 1), overlay.height() - 1
		);
		painter.drawLine(
			right_line(0), 0,
			right_line(overlay.height() - 1), overlay.height() - 1
		);

		// draw camera y to Z-Coordinate mapping function
		auto height_diff = height_max - height_min;
		painter.setPen(QPen(QBrush(qRgb(192, 192, 0)), 2));
		auto factor = std::size_t(bitmap_.cols()) / height_diff;
		auto w = std::size_t(bitmap_.cols());

		using namespace mitrax::literals;
		for(std::size_t i = 0; i < bitmap_.rows() - 1_R; ++i){
			auto y = std::size_t(bitmap_.rows()) - i - 1;
			painter.drawLine(
				(y_to_Z(y) - height_min) * factor, y,
				(y_to_Z(y - 1) - height_min) * factor, y + 1
			);
		}

		// Z-Coordinate is drawn in x-direction, draw min and max as text
		QFont font = painter.font();
		font.setPixelSize(24);
		painter.setFont(font);
		painter.drawText(QRect(
			QPoint(2, overlay.height() - 32),
			QPoint(w / 2, overlay.height())),
			Qt::AlignLeft | Qt::AlignVCenter,
			tr("Z = %L1 µm").arg(height_min)
		);
		painter.drawText(QRect(
			QPoint(w / 2, overlay.height() - 32),
			QPoint(w - 2, overlay.height())),
			Qt::AlignRight | Qt::AlignVCenter,
			tr("Z = %L1 µm").arg(height_max)
		);

		return overlay;
	}

	void widget_calib::analyze_x(){
// 		if(exception_catcher([this]{
// 			std::vector< mitrax::point< double > > left_points;
// 			std::vector< mitrax::point< double > > right_points;
// 			std::vector< mitrax::point< double > > y_to_Z_points;
// 			left_points.reserve(laser_calib_.size());
// 			right_points.reserve(laser_calib_.size());
// 			y_to_Z_points.reserve(laser_calib_.size());
// 			for(auto const& v: laser_calib_){
// 				left_points.emplace_back(v.y, v.x1);
// 				right_points.emplace_back(v.y, v.x2);
// 				y_to_Z_points.emplace_back(v.y, v.Z);
// 			}
// 
// 			image_.set_images(to_image(bitmap_), draw_yz(
// 				left_points, right_points, y_to_Z_points,
// 				laser_calib_.front().Z, laser_calib_.back().Z
// 			));
// 
// 	// 		laser_calibration calib;
// 	// 		calib.set(y_to_Z, left_line, right_line);
// 	// 		set_laser_calib_(calib);
// 		})){
// 			set_step(step::calib_yz);
// 			stop();
// 		}else{
// 			reset();
// 		}
	}

	void widget_calib::analyze_yz(){
		if(exception_catcher([this]{
			std::vector< mitrax::point< double > > left_points;
			std::vector< mitrax::point< double > > right_points;
			std::vector< mitrax::point< double > > y_to_Z_points;
			left_points.reserve(circle_calib_.size());
			right_points.reserve(circle_calib_.size());
			y_to_Z_points.reserve(circle_calib_.size());
			for(auto const& v: circle_calib_){
				left_points.emplace_back(v.c1.y(), v.c1.x());
				right_points.emplace_back(v.c2.y(), v.c2.x());
				y_to_Z_points.emplace_back(v.y, v.Z);
			}

			image_.set_images(to_image(bitmap_), draw_yz(
				left_points, right_points, y_to_Z_points,
				circle_calib_.front().Z, circle_calib_.back().Z
			));

	// 		laser_calibration calib;
	// 		calib.set(y_to_Z, left_line, right_line);
	// 		set_laser_calib_(calib);
		})){
			set_step(step::complete);
			stop();
		}else{
			reset();
		}
	}

	void widget_calib::align_ready(){
		set_step(step::calib_yz);
	}

	void widget_calib::reset(){
		set_step(step::align);
		stop();
		image_.start_live();
	}

	void widget_calib::set_running(bool is_running){
		auto set_enabled = [this](bool enabled){
			original_.setEnabled(enabled);
			binarized_.setEnabled(enabled);
			eroded_.setEnabled(enabled);
			binarize_threashold_l_.setEnabled(enabled);
			erode_l_.setEnabled(enabled);
			binarize_threashold_.setEnabled(enabled);
			erode_.setEnabled(enabled);
			laser_line_.setEnabled(enabled);
			laser_auto_stop_l_.setEnabled(!enabled);
			laser_auto_stop_.setEnabled(!enabled);
		};

		running_ = is_running;

		if(is_running){
			switch(step_){
				case step::align:
					break;
				case step::calib_yz:
					null_pos_ = mcl3_.position();
					bitmap_ = mitrax::make_bitmap_by_default< std::uint8_t >(
						cam_.cols(), cam_.rows(), 255
					);
					circle_calib_.clear();
					break;
				case step::calib_x:
					break;
				case step::complete:
					reset();
					return;
			}

			exception_count_ = 0;
			save_count_line_ = 0;

			image_.stop_live();
			timer_.start(0);
			laser_start_.setText(tr("Stop"));

			radio_buttons_.setExclusive(false);
			original_.setChecked(false);
			binarized_.setChecked(false);
			eroded_.setChecked(false);
			laser_line_.setChecked(false);
			radio_buttons_.setExclusive(true);

			set_enabled(false);
		}else{
			timer_.stop();
			laser_start_.setText(tr("Start"));

			laser_auto_stop_.setChecked(false);
			set_enabled(true);

			mcl3_.move_to(null_pos_[0], null_pos_[1], null_pos_[2]);
		}
	}

	void widget_calib::set_step(step s){
		auto text = tr("Step %1 from 2");
		step_ = s;
		switch(step_){
			case step::align:{
// 				step_l_.setText(text.arg(1));
// 				laser_start_.setText(tr("Start"));
				break;
			}

			case step::calib_yz:{
				step_l_.setText(text.arg(2));
				laser_start_.setText(tr("Start"));
				break;
			}

			case step::calib_x:{
// 				step_l_.setText(text.arg(1));
// 				laser_start_.setText(tr("Start"));
				break;
			}

			case step::complete:{
				step_l_.setText(tr("complete"));
				laser_start_.setText(tr("Reset"));
				break;
			}
		}
	}


}
