//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widgetdock_calc_laser_line.hpp>
#include <linescan/calc_laser_line.hpp>


namespace linescan{


	widgetdock_calc_laser_line::widgetdock_calc_laser_line():
		QDockWidget(tr("Laser line")),
		method_(tr("Calculation method")),
		method_threshold_(tr("Threshold")),
		method_sum_(tr("Sum")),
		threshold_(tr("Settings")),
		threshold_show_original_(tr("Show original")),
		threshold_show_binarize_(tr("Show binarized")),
		threshold_binarize_l_(tr("Threshold: ")),
		threshold_show_erode_(tr("Show eroded")),
		threshold_erode_l_(tr("Erode: ")),
		threshold_show_line_(tr("Show line")),
		threshold_subpixel_l_(tr("Subpixel: ")),
		sum_(tr("Settings")),
		sum_show_original_(tr("Show original")),
		sum_min_value_l_(tr("Minimal value: ")),
		sum_min_sum_l_(tr("Minimal sum: ")),
		sum_show_line_(tr("Show line")),
		sum_subpixel_l_(tr("Subpixel: "))
	{
		method_layout_.addWidget(&method_sum_);
		method_layout_.addWidget(&method_threshold_);

		method_sum_.setChecked(true);

		method_.setLayout(&method_layout_);


		threshold_binarize_.setRange(1, 255);
		threshold_erode_.setRange(0, 10);

		threshold_layout_.addWidget(&threshold_show_original_, 0, 0, 1, 2);
		threshold_layout_.addWidget(&threshold_show_binarize_, 1, 0, 1, 2);
		threshold_layout_.addWidget(&threshold_binarize_l_, 2, 0, 1, 1);
		threshold_layout_.addWidget(&threshold_binarize_, 2, 1, 1, 1);
		threshold_layout_.addWidget(&threshold_show_erode_, 3, 0, 1, 2);
		threshold_layout_.addWidget(&threshold_erode_l_, 4, 0, 1, 1);
		threshold_layout_.addWidget(&threshold_erode_, 4, 1, 1, 1);
		threshold_layout_.addWidget(&threshold_show_line_, 5, 0, 1, 2);
		threshold_layout_.addWidget(&threshold_subpixel_l_, 6, 0, 1, 1);
		threshold_layout_.addWidget(&threshold_subpixel_, 6, 1, 1, 1);

		threshold_show_line_.setChecked(true);

		threshold_binarize_.setValue(255);
		threshold_erode_.setValue(2);
		threshold_subpixel_.setChecked(true);

		threshold_binarize_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		threshold_erode_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		threshold_subpixel_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		threshold_.setLayout(&threshold_layout_);


		sum_min_value_.setRange(0, 254);
		sum_min_sum_.setRange(0, 200'000);

		sum_layout_.addWidget(&sum_show_original_, 0, 0, 1, 2);
		sum_layout_.addWidget(&sum_min_value_l_, 1, 0, 1, 1);
		sum_layout_.addWidget(&sum_min_value_, 1, 1, 1, 1);
		sum_layout_.addWidget(&sum_min_sum_l_, 2, 0, 1, 1);
		sum_layout_.addWidget(&sum_min_sum_, 2, 1, 1, 1);
		sum_layout_.addWidget(&sum_show_line_, 3, 0, 1, 2);
		sum_layout_.addWidget(&sum_subpixel_l_, 4, 0, 1, 1);
		sum_layout_.addWidget(&sum_subpixel_, 4, 1, 1, 1);

		sum_show_line_.setChecked(true);

		sum_min_value_.setValue(20);
		sum_min_sum_.setValue(4'000);
		sum_subpixel_.setChecked(true);

		sum_min_value_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		sum_min_sum_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		sum_subpixel_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		sum_.setLayout(&sum_layout_);


		main_layout_.addWidget(&method_);
		main_layout_.addWidget(&threshold_);
		main_layout_.addWidget(&sum_);
		main_layout_.addStretch();

		main_widget_.setLayout(&main_layout_);
		setWidget(&main_widget_);


		sum_.hide();


		constexpr auto released = &QRadioButton::released;

		constexpr auto check_released = &QAbstractButton::released;

		constexpr auto valueChanged =
			static_cast< void(QSpinBox::*)(int) >(&QSpinBox::valueChanged);

		auto const update_threshold_v = [this]{ update_threshold(); };
		auto const update_threshold_i = [this](int){ update_threshold(); };

		auto const update_sum_v = [this]{ update_sum(); };
		auto const update_sum_i = [this](int){ update_sum(); };


		connect(&method_threshold_, released, [this]{
			show_box(threshold_);
			update_threshold();
		});

		connect(&method_sum_, released, [this]{
			show_box(sum_);
			update_sum();
		});


		connect(&threshold_binarize_, valueChanged, update_threshold_i);
		connect(&threshold_erode_, valueChanged, update_threshold_i);

		connect(&threshold_subpixel_, check_released, update_threshold_v);

		connect(&threshold_show_original_, released, update_threshold_v);
		connect(&threshold_show_binarize_, released, update_threshold_v);
		connect(&threshold_show_erode_, released, update_threshold_v);
		connect(&threshold_show_line_, released, update_threshold_v);

		connect(&sum_min_value_, valueChanged, update_sum_i);
		connect(&sum_min_sum_, valueChanged, update_sum_i);

		connect(&sum_subpixel_, check_released, update_sum_v);

		connect(&sum_show_original_, released, update_sum_v);
		connect(&sum_show_line_, released, update_sum_v);

		update_threshold();
		update_sum();
	}

	void widgetdock_calc_laser_line::show_box(QGroupBox& box){
		for(auto pointer: {
			&threshold_,
			&sum_
		}) pointer->setVisible(pointer == &box);
	}

	void widgetdock_calc_laser_line::update_threshold()const{
		calc_laser_line.use(
			[this]{
				if(threshold_show_original_.isChecked()){
					return calc_laser_line_mode::threshold::original;
				}
				if(threshold_show_binarize_.isChecked()){
					return calc_laser_line_mode::threshold::binarize;
				}
				if(threshold_show_erode_.isChecked()){
					return calc_laser_line_mode::threshold::erode;
				}
				return calc_laser_line_mode::threshold::line;
			}(),
			threshold_binarize_.value(),
			threshold_erode_.value(),
			threshold_subpixel_.isChecked()
		);
	}

	void widgetdock_calc_laser_line::update_sum()const{
		calc_laser_line.use(
			[this]{
				if(sum_show_original_.isChecked()){
					return calc_laser_line_mode::sum::original;
				}
				return calc_laser_line_mode::sum::line;
			}(),
			sum_min_value_.value(),
			sum_min_sum_.value(),
			sum_subpixel_.isChecked()
		);
	}


}
