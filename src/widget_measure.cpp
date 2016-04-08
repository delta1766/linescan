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
#include <linescan/block_signals.hpp>

#include <fstream>


namespace linescan{


	widget_measure::widget_measure(
		camera& cam,
		control_F9S_MCL3& mcl3
	):
		cam_(cam),
		mcl3_(mcl3),
		y_l_("<b>" + tr("primary direction (Y)") + "</b>"),
		y_from_l_(tr("From: ")),
		y_to_l_(tr("To: ")),
		y_step_l_(tr("Step: ")),
		x_l_("<b>" + tr("secondary direction (X)") + "</b>"),
		x_from_l_(tr("From: ")),
		x_to_l_(tr("To: ")),
		x_step_l_(tr("Step: ")),
		start_(tr("Start measurement")),
		save_(tr("Save image")),
		image_(cam)
	{
		main_layout_.addWidget(&y_l_, 0, 0, 1, 2);
		main_layout_.addWidget(&y_from_l_, 1, 0, 1, 1);
		main_layout_.addWidget(&y_from_, 1, 1, 1, 1);
		main_layout_.addWidget(&y_to_l_, 2, 0, 1, 1);
		main_layout_.addWidget(&y_to_, 2, 1, 1, 1);
		main_layout_.addWidget(&y_step_l_, 3, 0, 1, 1);
		main_layout_.addWidget(&y_step_, 3, 1, 1, 1);

		main_layout_.addWidget(&x_l_, 4, 0, 1, 2);
		main_layout_.addWidget(&x_from_l_, 5, 0, 1, 1);
		main_layout_.addWidget(&x_from_, 5, 1, 1, 1);
		main_layout_.addWidget(&x_to_l_, 6, 0, 1, 1);
		main_layout_.addWidget(&x_to_, 6, 1, 1, 1);
		main_layout_.addWidget(&x_step_l_, 7, 0, 1, 1);
		main_layout_.addWidget(&x_step_, 7, 1, 1, 1);

		main_layout_.addWidget(&start_, 8, 0, 1, 2);
		main_layout_.addWidget(&save_, 9, 0, 1, 2);
		main_layout_.setRowStretch(10, 1);

		layout_.addLayout(&main_layout_);
		layout_.addWidget(&image_);

		setLayout(&layout_);


		y_l_.setAlignment(Qt::AlignCenter);
		x_l_.setAlignment(Qt::AlignCenter);

		y_from_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		y_to_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		y_step_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		x_from_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		x_to_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		x_step_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);


		y_from_.setSuffix(tr(" mm"));
		y_to_.setSuffix(tr(" mm"));
		y_step_.setSuffix(tr(" mm"));

		x_from_.setSuffix(tr(" mm"));
		x_to_.setSuffix(tr(" mm"));
		x_step_.setSuffix(tr(" mm"));

		y_from_.setDecimals(3);
		y_to_.setDecimals(3);
		y_step_.setDecimals(3);
		y_step_.setSingleStep(0.1);

		x_from_.setDecimals(3);
		x_to_.setDecimals(3);
		x_step_.setDecimals(3);
		x_from_.setSingleStep(15);
		x_to_.setSingleStep(15);
		x_step_.setSingleStep(15);

		y_from_.setRange(-500, 500);
		y_to_.setRange(-500, 500);
		y_step_.setRange(-100, 100);

		x_from_.setRange(-500, 500);
		x_to_.setRange(-500, 500);
		x_step_.setRange(-100, 100);

		y_from_.setValue(0);
		y_to_.setValue(-50);
		y_step_.setValue(-0.1);

		x_from_.setValue(0);
		x_to_.setValue(0);
		x_step_.setValue(15);


		start_.setCheckable(true);
		start_.setEnabled(false);


		timer_.setSingleShot(true);


		points_.reserve(1'000'000);


		image_.set_processor([this](auto&& image){
			return calc_laser_line(image, as_image);
		});


		auto set_enabled = [this](bool on){
			y_from_.setEnabled(on);
			y_to_.setEnabled(on);
			y_step_.setEnabled(on);
			x_from_.setEnabled(on);
			x_to_.setEnabled(on);
			x_step_.setEnabled(on);
		};


		connect(&save_, &QPushButton::released, [this]{
			auto image = image_.image();

			auto name = QString("live_%1.png")
				.arg(image_save_count_, 4, 10, QLatin1Char('0'));

			++image_save_count_;

			message(tr("Save image '%1'.").arg(name));
			image.save(name, "PNG");
		});

		struct range_checker{
			void operator()(double /*dummy*/ = 0){
				auto block_f = block_signals(from);
				auto block_t = block_signals(to);
				auto block_s = block_signals(step);

				auto f = from.value();
				auto t = to.value();
				auto s = step.value();
				auto s2 = step2.value();

				start.setEnabled(
					s != 0 && s2 != 0 && calib.is_valid()
				);

				if(s >= 0){
					auto new_f = std::min(f, t);
					auto new_t = std::max(f, t);

					from.setValue(new_f);
					to.setValue(new_t);

					from.setRange(-500, new_t);
					to.setRange(new_f, 500);
				}else{
					auto new_f = std::max(f, t);
					auto new_t = std::min(f, t);

					from.setValue(new_f);
					to.setValue(new_t);

					from.setRange(new_t, 500);
					to.setRange(-500, new_f);
				}
			}

			QDoubleSpinBox& from;
			QDoubleSpinBox& to;
			QDoubleSpinBox& step;

			QPushButton& start;
			calibration& calib;
			QDoubleSpinBox& step2;
		};

		constexpr auto value_changed =
			static_cast< void(QDoubleSpinBox::*)(double) >
			(&QDoubleSpinBox::valueChanged);

		auto y_range_checker =
			range_checker{y_from_, y_to_, y_step_, start_, calib_, x_step_};
		connect(&y_from_, value_changed, y_range_checker);
		connect(&y_to_, value_changed, y_range_checker);
		connect(&y_step_, value_changed, y_range_checker);
		y_range_checker();

		auto x_range_checker =
			range_checker{x_from_, x_to_, x_step_, start_, calib_, y_step_};
		connect(&x_from_, value_changed, x_range_checker);
		connect(&x_to_, value_changed, x_range_checker);
		connect(&x_step_, value_changed, x_range_checker);
		x_range_checker();

		connect(&start_, &QPushButton::released, [this, set_enabled]{
			if(start_.isChecked()){
				start_.setText(tr("Stop measurement"));
				set_enabled(false);

				exception_count_ = 0;

				points_.clear();
				image_.stop_live();
				timer_.start(0);
			}else{
				timer_.stop();
				start_.setText(tr("Start measurement"));
				set_enabled(true);

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

				auto X = mcl3_.read_x() / 1000.;
				auto Y = mcl3_.read_y() / 1000.;
				auto Z = mcl3_.read_z() / 1000.;
				for(auto const& p: points){
					points_.push_back(std::array< double, 3 >{{
						X + calib_.X(p.x(), p.y()),
						Y,
						Z + calib_.Z(p.y())
					}});
				}
			});

			exception |= !exception_catcher([&]{
				mcl3_.move_relative(0, -100, 0);
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
