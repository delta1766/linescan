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


namespace linescan{


	widget_measure::widget_measure(
		camera& cam,
		control_F9S_MCL3& mcl3
	):
		cam_(cam),
		mcl3_(mcl3),
		save_(tr("Save")),
		image_(cam)
	{
		(void)cam_;
		(void)mcl3_;

		main_layout_.addWidget(&save_, 0, 0, 1, 2);
		main_layout_.setRowStretch(1, 1);

		layout_.addLayout(&main_layout_);
		layout_.addWidget(&image_);

		setLayout(&layout_);


		image_.set_processor([this](auto&& image){
			return calc_laser_line(image, as_image);
		});


		connect(&save_, &QPushButton::released, [this]{
			auto image = image_.image();

			auto name = QString("live_%1.png")
				.arg(save_count_, 4, 10, QLatin1Char('0'));

			++save_count_;

			message(tr("Save image '%1'.").arg(name));
			image.save(name, "PNG");
		});
	}


}
