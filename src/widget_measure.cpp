//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_measure.hpp>
#include <linescan/calc_laser_line.hpp>
#include <linescan/exception_catcher.hpp>

#include <fstream>


namespace linescan{


	widget_measure::widget_measure(
		camera& cam,
		control_F9S_MCL3& mcl3
	):
		cam_(cam),
		mcl3_(mcl3),
		start_(tr("Start")),
		save_(tr("Save")),
		image_(cam)
	{
		main_layout_.addWidget(&start_, 0, 0, 1, 2);
		main_layout_.addWidget(&save_, 1, 0, 1, 2);
		main_layout_.setRowStretch(2, 1);

		layout_.addLayout(&main_layout_);
		layout_.addWidget(&image_);

		setLayout(&layout_);

		start_.setCheckable(true);
		start_.setEnabled(false);


		timer_.setSingleShot(true);


		points_.reserve(1'000'000);


		image_.set_processor([this](auto&& image){
			return calc_laser_line(image, as_image);
		});


		connect(&save_, &QPushButton::released, [this]{
			auto image = image_.image();

			auto name = QString("live_%1.png")
				.arg(image_save_count_, 4, 10, QLatin1Char('0'));

			++image_save_count_;

			message(tr("Save image '%1'.").arg(name));
			image.save(name, "PNG");
		});

		connect(&start_, &QPushButton::released, [this]{
			if(start_.isChecked()){

				start_.setText(tr("Stop"));

				exception_count_ = 0;

				points_.clear();
				image_.stop_live();
				timer_.start(0);
			}else{
				timer_.stop();
				start_.setText(tr("Start"));

				auto name = QString("measure_%1.asc")
					.arg(measure_save_count_, 4, 10, QLatin1Char('0'));

				++measure_save_count_;

				message(tr("Save measurement '%1'.").arg(name));

				std::ofstream os(name.toStdString().c_str());
				for(auto const& p: points_){
					os << p[0] << ' ' << p[1] << ' ' << p[2] << '\n';
				}

				image_.start_live();
			}
		});

		connect(&timer_, &QTimer::timeout, [this]{
			bool exception = false;

			exception |= !exception_catcher([&]{
				std::vector< mitrax::point< double > > points;
				QImage image;
				std::tie(points, image) =
					calc_laser_line(cam_.image(), points_and_image);

				image_.set_images(image, QImage());

				auto Y = mcl3_.read_y();
				for(auto const& p: points){
					points_.push_back(std::array< double, 3 >{{
						calib_.X(p.x(), p.y()),
						Y / 1000.,
						calib_.Z(p.y())
					}});
				}
			});

			exception |= !exception_catcher([&]{
				mcl3_.move_relative(0, 100, 0);
			});

			if(exception){
				++exception_count_;
			}else{
				exception_count_ = 0;
			}

			if(exception_count_ > 9){
				QMessageBox box(
					QMessageBox::Warning,
					QObject::tr("Error"),
					tr("%1 errors back-to-back, stop measure?")
						.arg(exception_count_),
					QMessageBox::Yes | QMessageBox::No
				);

				exception_count_ = 0;

				if(box.exec() == QMessageBox::Yes) start_.click();
			}

			timer_.start(1);
		});
	}

	widget_measure::~widget_measure(){
		timer_.disconnect();
	}

	void widget_measure::set_calibration(calibration const& calib){
		calib_ = calib;
		if(calib_.is_valid()) start_.setEnabled(true);
	}


}
