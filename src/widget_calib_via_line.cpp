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
#include <linescan/calc_top_distance_line.hpp>
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
		control_F9S_MCL3& mcl3
	):
		widget_processing_base(cam),
		cam_(cam),
		mcl3_(mcl3),
		height_(0),
		save_count_line_(0),
		line_(tr("Line image")),
		laser_start_(tr("Start")),
		laser_auto_stop_l_(tr("Auto stop")),
		running_(false)
	{
#ifndef MCL
		(void)mcl3_;
#endif

		glayout_.addWidget(&line_, 5, 0, 1, 2);
		glayout_.addWidget(&laser_start_, 6, 0, 1, 2);
		glayout_.addWidget(&laser_auto_stop_l_, 7, 0, 1, 1);
		glayout_.addWidget(&laser_auto_stop_, 7, 1, 1, 1);
		glayout_.setRowStretch(8, 1);

		connect(&line_, &QRadioButton::released, [this]{
			if(!line_.isChecked()) return;

			image_.set_processor(
				[this](mitrax::raw_bitmap< std::uint8_t >&& bitmap){
					auto overlay = draw_laser_alignment(
						bitmap, get_threashold(), get_erode()
					);

					return std::pair< QImage, QImage >(
						to_image(std::move(bitmap)), overlay
					);
				});
		});

		connect(&laser_start_, &QPushButton::released, [this]{
			if(running_){
				analyze();
			}else{
				start();
			}
		});

		connect(&timer_, &QTimer::timeout, [this]{
			exception_catcher([&]{
				auto name = QString("data/laser/laser_%1.png")
					.arg(save_count_line_, 4, 10, QLatin1Char('0'))
					.toStdString();

				++save_count_line_;
// 				save_count_line_ += 10;

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

				auto points = calc_top_distance_line(
					image, get_threashold(), get_erode()
				);

				image_.set_images(
					to_image(bitmap_),
					draw_laser_alignment(bitmap_.dims(), points)
				);

				auto line = fit_polynom< 1 >(points);

				std::cout << height_ << std::endl;
				y_to_height_points_.emplace_back(
					line(std::size_t(bitmap_.cols()) / 2), height_
				);

#ifdef MCL
				mcl3_.move_relative(0, 0, 100);
#endif
				height_ += 100;
// 				height_ += 1000;
			}, false);

			if(
				y_to_height_points_.size() > 0 &&
				laser_auto_stop_.isChecked() &&
				y_to_height_points_.back().x() <= 
				std::size_t(bitmap_.rows()) - y_to_height_points_.front().x()
			){
				analyze();
				return;
			}

			if(running_) timer_.start(1);
		});

		laser_auto_stop_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		laser_auto_stop_l_.setEnabled(false);
		laser_auto_stop_.setEnabled(false);
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

	void widget_calib_via_line::analyze(){
		stop();

		auto y_to_height_ = fit_polynom< 3 >(y_to_height_points_);

		auto binary = eroded(bitmap_);

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

		auto left_laser_line_ = fit_polynom< 1 >(left_points);
		auto right_laser_line_ = fit_polynom< 1 >(right_points);

		std::cout
			<< y_to_height_[3] << " * x^3 + "
			<< y_to_height_[2] << " * x^2 + "
			<< y_to_height_[1] << " * x + "
			<< y_to_height_[0] << std::endl;
		std::cout
			<< left_laser_line_[1] << " * x + "
			<< left_laser_line_[0] << std::endl;
		std::cout
			<< right_laser_line_[1] << " * x + "
			<< right_laser_line_[0] << std::endl;
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
			line_.setEnabled(enabled);
			laser_auto_stop_l_.setEnabled(!enabled);
			laser_auto_stop_.setEnabled(!enabled);
		};

		if(is_running){
			bitmap_ = mitrax::make_bitmap_by_default< std::uint8_t >(
				cam_.cols(), cam_.rows()
			);
			y_to_height_points_.clear();
			height_ = 0;

			save_count_line_ = 0;

			image_.stop_live();
			timer_.start(0);
			laser_start_.setText(tr("Stop"));

			set_enabled(false);
		}else{
			timer_.stop();
			image_.start_live();
			laser_start_.setText(tr("Start"));

			laser_auto_stop_.setChecked(false);
			set_enabled(true);
		}

		running_ = is_running;
	}


}
