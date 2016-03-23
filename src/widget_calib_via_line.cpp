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

// 				save(bitmap_, "calib_via_line.png");
// 				{
// 					std::ofstream os("calib_via_line.txt");
// 					for(auto const& v: top_distance_to_height_){
// 						os << v.x() << ' ' << v.y() << '\n';
// 					}
// 				}

				top_distance_to_height_.clear();
				{
					std::ifstream is("calib_via_line.txt");
					for(double a, b; is >> a >> b;){
						top_distance_to_height_.emplace_back(a, b);
					}
				}

				{
					auto f = fit_polynom< 3 >(top_distance_to_height_);
					QMessageBox box(
						QMessageBox::Warning,
						QObject::tr("Error"),
						QString("%1 * x^3 + %2 * x^2 + %3 * x^1 + %4")
							.arg(f[3]).arg(f[2]).arg(f[1]).arg(f[0]),
						QMessageBox::Ok
					);
// 					box.exec();
				}

				bitmap_ = load("calib_via_line.png");

				auto binary = eroded(bitmap_);
				save(binary, "result.png");

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

				auto left_line = fit_polynom< 1 >(left_points);
				auto right_line = fit_polynom< 1 >(right_points);

				auto image = to_image(bitmap_)
					.convertToFormat(QImage::Format_RGB32);

				{
					QPainter painter(&image);
					painter.setRenderHint(QPainter::Antialiasing, true);

					painter.setPen(QPen(QBrush(qRgb(255, 0, 0)), 3));

					painter.drawLine(
						left_line(0), 0,
						left_line(binary.rows()), binary.rows()
					);

					painter.drawLine(
						right_line(0), 0,
						right_line(binary.rows()), binary.rows()
					);
				}

				image.save("dummy.png", "PNG");
			}else{
				start();
			}
		});

		connect(&timer_, &QTimer::timeout, [this]{
			exception_catcher([&]{
				auto image = cam_.image();

				static std::size_t save_count_ = 0;
				auto name = QString("laser_%1.png")
					.arg(save_count_, 4, 10, QLatin1Char('0')).toStdString();

				++save_count_;

				save(image, name);
// 				bitmap_ = mitrax::transform([](auto a, auto b){
// 					return std::min(a, b);
// 				}, bitmap_, image);

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
		if(is_running){
			bitmap_ = mitrax::make_bitmap_by_default< std::uint8_t >(
				cam_.cols(), cam_.rows()
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
		}

		running_ = is_running;
	}


}
