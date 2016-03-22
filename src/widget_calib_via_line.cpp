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
#include <linescan/polynom.hpp>
#include <linescan/to_image.hpp>
#include <linescan/save.hpp>
#include <linescan/draw.hpp>

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
			}else{
				start();
			}
		});

		connect(&timer_, &QTimer::timeout, [this]{
			exception_catcher([&]{
				auto image = cam_.image();

				bitmap_ = mitrax::transform([](auto a, auto b){
					return std::max(a, b);
				}, bitmap_, image);

				auto top_distance_line = calc_top_distance_line(
					image, get_threashold(), get_erode()
				);

				image_.set_images(
					to_image(bitmap_),
					draw_laser_alignment(bitmap_.dims(), top_distance_line)
				);

				std::vector< mitrax::point< float > > points;
				for(std::size_t i = 0; i < top_distance_line.size(); ++i){
					if(top_distance_line[i] == 0) continue;
					points.emplace_back(i, top_distance_line[i]);
				}

				auto line = fit_polynom< 1 >(points);

				top_distance_to_height_.push_back({{
					line(std::size_t(bitmap_.cols()) / 2), height_
				}});

				mcl3_.move_relative(0, 0, 100);
				height_ += 100;
			});

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
		if(is_running){
			bitmap_ = mitrax::make_matrix(
				mitrax::dims(cam_.cols(), cam_.rows()), std::uint8_t()
			);
			top_distance_to_height_.clear();
			height_ = 0;

			image_.stop_live();
			timer_.start(0);
			start_.setText(tr("Stop"));
		}else{
			timer_.stop();
			image_.start_live();
			start_.setText(tr("Start"));

			save(bitmap_, "calib.png");
			{
				std::ofstream os("calib.txt");
				for(auto const& v: top_distance_to_height_){
					os << v[0] << ", " << v[1] << '\n';
				}
			}
		}

		running_ = is_running;
	}


}
