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


namespace linescan{


	widget_calib_via_line::widget_calib_via_line(
		camera& cam,
		control_F9S_MCL3& mcl3
	):
		widget_processing_base(cam),
		cam_(cam),
		mcl3_(mcl3),
		line_(tr("Line image")),
		start_(tr("Start")),
		running_(false)
	{
		glayout_.addWidget(&line_, 5, 0, 1, 2);
		glayout_.addWidget(&start_, 6, 0, 1, 2);
		glayout_.setRowStretch(7, 1);

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

		connect(&start_, &QPushButton::released, [this]{
			if(running_){
				stop();

				auto y_to_height_ = fit_polynom< 3 >(top_distance_to_height_);

				auto binary = eroded(bitmap_);

				std::vector< mitrax::point< double > > left_points;
				std::vector< mitrax::point< double > > right_points;
				std::size_t mid_x = std::size_t(binary.cols()) / 2;
				std::size_t from_y = std::size_t(binary.rows()) * 2 / 100;
				std::size_t to_y = std::size_t(binary.rows()) * 98 / 100;
				for(std::size_t y = from_y; y < to_y; ++y){
					if(!binary(mid_x, y)) continue;

					for(std::size_t x = mid_x; x > 0; --x){
						if(!binary(x, y)){
							left_points.emplace_back(y, x);
							break;
						}
					}

					for(std::size_t x = mid_x; x < binary.cols(); ++x){
						if(!binary(x, y)){
							right_points.emplace_back(y, x);
							break;
						}
					}
				}

				auto left_laser_line_ = fit_polynom< 1 >(left_points);
				auto right_laser_line_ = fit_polynom< 1 >(right_points);
			}else{
				save_count_line_ = 0;
				start();
			}
		});

		connect(&timer_, &QTimer::timeout, [this]{
			exception_catcher([&]{
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

				auto points = calc_top_distance_line(
					image, get_threashold(), get_erode()
				);

				image_.set_images(
					to_image(bitmap_),
					draw_laser_alignment(bitmap_.dims(), points)
				);

				auto line = fit_polynom< 1 >(points);

				top_distance_to_height_.emplace_back(
					line(std::size_t(bitmap_.cols()) / 2), height_
				);

				mcl3_.move_relative(0, 0, 100);
				height_ += 100;
			}, false);

			if(running_) timer_.start(1);
		});
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
		};

		if(is_running){
			bitmap_ = mitrax::make_bitmap_by_default< std::uint8_t >(
				cam_.cols(), cam_.rows()
			);
			top_distance_to_height_.clear();
			height_ = 0;

			image_.stop_live();
			timer_.start(0);
			start_.setText(tr("Stop"));

			set_enabled(false);
		}else{
			timer_.stop();
			image_.start_live();
			start_.setText(tr("Start"));

			set_enabled(true);
		}

		running_ = is_running;
	}


}
