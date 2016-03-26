//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_calib_via_line.hpp>
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


	widget_calib_via_line::widget_calib_via_line(
		camera& cam,
		control_F9S_MCL3& mcl3,
		std::function< void(laser_calibration const&) > set_laser_calib
	):
		widget_processing_base(cam),
		cam_(cam),
		mcl3_(mcl3),
		set_laser_calib_(set_laser_calib),
		height_(0),
		target_differences_{{{0, 0}, {0, 0}}},
		save_count_line_(0),
		laser_line_(tr("Laser image")),
		laser_auto_stop_l_(tr("Auto stop")),
		running_(false)
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

			if(step_ != step::target){
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
					case step::laser: analyze_laser(); break;
					case step::target: analyze_target(); break;
					case step::complete: reset(); break;
				}
			}else{
				start();
			}
		});

		connect(&timer_, &QTimer::timeout, [this]{
			if(step_ == step::laser) exception_catcher([&]{
				auto name = QString("data/laser/laser_%1.png")
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
					return std::max(a, b);
				}, bitmap_, image);

				auto points = calc_laser_line(
					image, get_threashold(), get_erode()
				);

				auto line = fit_polynom< 1 >(points);

				image_.set_images(
					to_image(bitmap_),
					draw_line(bitmap_.dims(), line)
				);

				y_to_height_points_.emplace_back(
					line(std::size_t(bitmap_.cols()) / 2), height_
				);
			}, false);

			if(step_ == step::target) exception_catcher([&]{
				auto name = QString("data/target/laser_%1.png")
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

				auto circles = last_circles_.size() == 2 ?
					find_calib_circles(
						image, last_circles_[0], last_circles_[1]
					) : find_calib_circles(image);

				if(last_circles_.size() == 2){
					last_circles_[0].x() += target_differences_[0].x();
					last_circles_[0].y() += target_differences_[0].y();
					last_circles_[1].x() += target_differences_[1].x();
					last_circles_[1].y() += target_differences_[1].y();
				}

				image_.set_images(
					to_image(bitmap_),
					draw_circle_line(image, circles)
				);

				if(circles.size() != 2 || circles[0].x() == circles[1].x()){
					return;
				}

				if(last_circles_.size() == 2 && (
					circles[0].radius() < last_circles_[0].radius() * 0.6 ||
					circles[1].radius() < last_circles_[1].radius() * 0.6 ||
					circles[0].radius() > last_circles_[0].radius() * 1.4 ||
					circles[1].radius() > last_circles_[1].radius() * 1.4
				)){
					return;
				}

				auto line =
					to_polynom(to_point(circles[0]), to_point(circles[1]));

				if(last_circles_.size() == 2){
					std::cout << (target_differences_[0].x() =
						circles[0].x() - last_circles_[0].x()) << std::endl;
					std::cout << (target_differences_[0].y() =
						circles[0].y() - last_circles_[0].y()) << std::endl;
					std::cout << (target_differences_[1].x() =
						circles[1].x() - last_circles_[1].x()) << std::endl;
					std::cout << (target_differences_[1].y() =
						circles[1].y() - last_circles_[1].y()) << std::endl;
				}

				last_circles_ = circles;

				y_to_height_points_.emplace_back(
					line(std::size_t(bitmap_.cols()) / 2), height_
				);
			}, false);

			exception_catcher([&]{
				mcl3_.move_relative(0, 0, 1000);
				height_ += 1000;
			}, false);

			if(
				y_to_height_points_.size() > 0 &&
				laser_auto_stop_.isChecked() &&
				y_to_height_points_.back().x() <= 
				std::size_t(bitmap_.rows()) - y_to_height_points_.front().x()
			){
				analyze_laser();
				return;
			}

			if(running_) timer_.start(1);
		});

		step_l_.setAlignment(Qt::AlignCenter);
		laser_auto_stop_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		laser_auto_stop_l_.setEnabled(false);
		laser_auto_stop_.setEnabled(false);

		set_step(step::laser);
	}

	bool widget_calib_via_line::is_running()const{
		return running_;
	}

	void widget_calib_via_line::stop(){
		set_running(false);
	}

	void widget_calib_via_line::start(){
		set_running(true);
	}

	void widget_calib_via_line::hideEvent(QHideEvent* event){
		QWidget::hideEvent(event);
		stop();
	}

	void widget_calib_via_line::analyze_laser(){
		auto y_to_height = fit_polynom< 3 >(y_to_height_points_);

		auto binary = erode(binarize(bitmap_, get_threashold()), get_erode());

		std::vector< mitrax::point< double > > left_points;
		std::vector< mitrax::point< double > > right_points;
		auto mid_x = std::size_t(binary.cols()) / 2;
		auto from_y = std::size_t(y_to_height_points_.back().x());
		auto to_y = std::size_t(y_to_height_points_.front().x());
		for(auto y = from_y; y < to_y; ++y){
			if(!binary(mid_x, y)) continue;

			for(auto x = mid_x; x > 0; --x){
				if(!binary(x, y)){
					left_points.emplace_back(y, x);
					break;
				}
			}

			for(auto x = mid_x; x < binary.cols(); ++x){
				if(!binary(x, y)){
					right_points.emplace_back(y, x);
					break;
				}
			}
		}

		auto left_laser_line = fit_polynom< 1 >(left_points);
		auto right_laser_line = fit_polynom< 1 >(right_points);

		// Create a transparent Overlay
		QImage overlay(bitmap_.cols(), bitmap_.rows(), QImage::Format_ARGB32);
		overlay.fill(0);

		{
			QPainter painter(&overlay);
			painter.setRenderHint(QPainter::Antialiasing, true);

			using namespace mitrax::literals;

			// Draw laser width functions
			painter.setPen(QPen(QBrush(qRgb(255, 0, 0)), 2));
			painter.drawLine(
				left_laser_line(0), 0,
				left_laser_line(overlay.height() - 1), overlay.height() - 1
			);
			painter.drawLine(
				right_laser_line(0), 0,
				right_laser_line(overlay.height() - 1), overlay.height() - 1
			);

			// draw camera y to Z-Coordinate mapping function
			auto max_height_ = y_to_height_points_.back().y();
			painter.setPen(QPen(QBrush(qRgb(192, 192, 0)), 2));
			auto factor = std::size_t(bitmap_.cols()) / max_height_;
			auto w = std::size_t(bitmap_.cols());

			using namespace mitrax::literals;
			for(std::size_t i = 0; i < bitmap_.rows() - 1_R; ++i){
				auto y = std::size_t(bitmap_.rows()) - i - 1;
				painter.drawLine(
					y_to_height(y) * factor, y,
					y_to_height(y - 1) * factor, y + 1
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
				tr("Z = %L1 µm").arg(0)
			);
			painter.drawText(QRect(
				QPoint(w / 2, overlay.height() - 32),
				QPoint(w - 2, overlay.height())),
				Qt::AlignRight | Qt::AlignVCenter,
				tr("Z = %L1 µm").arg(max_height_)
			);
		}

		image_.set_images(to_image(bitmap_), overlay);

		laser_calibration calib;
		calib.set(y_to_height, left_laser_line, right_laser_line);
		set_laser_calib_(calib);

		set_step(step::target);

		stop();
	}

	void widget_calib_via_line::analyze_target(){
		set_step(step::complete);

		stop();
	}

	void widget_calib_via_line::reset(){
		set_step(step::laser);

		stop();
	}

	void widget_calib_via_line::set_running(bool is_running){
		auto set_enabled = [this](bool enabled){
			original_.setEnabled(enabled);
			binarized_.setEnabled(enabled);
			eroded_.setEnabled(enabled);
			binarize_threashold_l_.setEnabled(enabled);
			erode_l_.setEnabled(enabled);
			sub_pixel_l_.setEnabled(enabled);
			binarize_threashold_.setEnabled(enabled);
			erode_.setEnabled(enabled);
			laser_line_.setEnabled(enabled);
			laser_auto_stop_l_.setEnabled(!enabled);
			laser_auto_stop_.setEnabled(!enabled);
		};

		running_ = is_running;

		if(is_running){
			switch(step_){
				case step::laser:
					bitmap_ = mitrax::make_bitmap_by_default< std::uint8_t >(
						cam_.cols(), cam_.rows()
					);
					break;
				case step::target:
					bitmap_ = mitrax::make_bitmap_by_default< std::uint8_t >(
						cam_.cols(), cam_.rows(), 255
					);
					last_circles_.clear();
					break;
				case step::complete: break;
			}
			y_to_height_points_.clear();
			height_ = 0;

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

			if(!y_to_height_points_.empty()){
				mcl3_.move_relative(0, 0, -y_to_height_points_.back().y());
			}
			y_to_height_points_.clear();
		}
	}

	void widget_calib_via_line::set_step(step s){
		auto text = tr("Step %1 from 2");
		step_ = s;
		switch(step_){
			case step::laser:{
				step_l_.setText(text.arg(1));
				laser_start_.setText(tr("Start"));
				break;
			}

			case step::target:{
				step_l_.setText(text.arg(2));
				laser_start_.setText(tr("Start"));
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
