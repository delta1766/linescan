//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 Benjamin Buch
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

#include <mitrax/transform.hpp>

#include <QSettings>


namespace linescan{


	widget_calib::widget_calib(camera& cam, control_F9S_MCL3& mcl3):
		cam_(cam),
		mcl3_(mcl3),
		image_(cam)
	{
		main_layout_.addWidget(&step_l_, 0, 0, 1, 2);
		main_layout_.addWidget(&laser_start_, 1, 0, 1, 2);
		main_layout_.setRowStretch(2, 1);

		layout_.addLayout(&main_layout_);
		layout_.addWidget(&image_);

		setLayout(&layout_);


		timer_.setSingleShot(true);


		connect(&laser_start_, &QPushButton::released, [this]{
			switch(step_){
				case step::align_laser:
					set_step(step::align_target);
					break;
				case step::align_target:
					set_step(step::calib_yz);
					break;
				case step::calib_yz:
					analyze_yz();
					break;
				case step::complete:
					reset();
					break;
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

					auto Z = mcl3_.read_z() / 1000.;
					return find_calib_circles(
						image,
						circle(
							fit_polynom< 3 >(x1)(Z),
							fit_polynom< 3 >(y1)(Z),
							circle_calib_.back().c1.radius()
						),
						circle(
							fit_polynom< 3 >(x2)(Z),
							fit_polynom< 3 >(y2)(Z),
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
					c[0], c[1], line(cam_.cols() / 2), mcl3_.read_z() / 1000.
				);
			}, false);

			exception |= !exception_catcher([&]{
				mcl3_.move_relative(0, 0, 1000);
			}, false);

			if(step_ == step::calib_yz && circle_calib_.size() > 1){
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

		set_step(step::align_laser);
	}

	widget_calib::~widget_calib(){
		timer_.disconnect();
	}

	void widget_calib::set_target_distance_in_mm(double value){
			target_distance_in_mm_ = value;
	}

	void widget_calib::stop(){
		running_ = false;

		timer_.stop();
		mcl3_.move_to(null_pos_[0], null_pos_[1], null_pos_[2]);
	}

	void widget_calib::start(){
		null_pos_ = mcl3_.position();

		bitmap_ = mitrax::make_matrix_v< std::uint8_t >(
			mitrax::cols(cam_.cols()), mitrax::rows(cam_.rows()), 255
		);

		circle_calib_.clear();

		exception_count_ = 0;
		save_count_line_ = 0;
		running_ = true;

		image_.stop_live();
		timer_.start(0);
	}

	void widget_calib::showEvent(QShowEvent* event){
		QWidget::showEvent(event);
		null_pos_ = mcl3_.position();
	}

	void widget_calib::hideEvent(QHideEvent* event){
		QWidget::hideEvent(event);
		reset();
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
			tr("Z = %L1 mm").arg(height_min)
		);
		painter.drawText(QRect(
			QPoint(w / 2, overlay.height() - 32),
			QPoint(w - 2, overlay.height())),
			Qt::AlignRight | Qt::AlignVCenter,
			tr("Z = %L1 mm").arg(height_max)
		);

		return overlay;
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

			auto left_line = fit_polynom< 1 >(left_points);
			auto right_line = fit_polynom< 1 >(right_points);
			auto y_to_Z = fit_polynom< 3 >(y_to_Z_points);

			calibration calib;
			calib.set(
				y_to_Z,
				left_line,
				(right_line - left_line) / target_distance_in_mm_
			);

			ready(calib);
		})){
			set_step(step::complete);
			stop();
		}else{
			reset();
		}
	}

	void widget_calib::reset(){
		set_step(step::align_laser);
		stop();
		image_.start_live();
	}

	void widget_calib::set_step(step s){
		auto text = tr("Step %1 from 3");
		step_ = s;
		switch(step_){
			case step::align_laser:{
				step_l_.setText(text.arg(1));
				laser_start_.setText(tr("Laser aligned"));

				image_.set_processor([this](auto&& image){
					auto pair = calc_laser_line(image, points_and_image);
					return std::pair< QImage, QImage >(
						pair.second,
						draw_laser_alignment(image.dims(), pair.first)
  					);
				});
				break;
			}

			case step::align_target:{
				step_l_.setText(text.arg(2));
				laser_start_.setText(tr("Target aligned"));

				image_.set_processor([this](auto&& image){
					return std::pair< QImage, QImage >(
						to_image(image),
						draw_circle_line(image)
  					);
				});
				break;
			}

			case step::calib_yz:{
				step_l_.setText(text.arg(3));
				laser_start_.setText(tr("Stop"));
				start();
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
