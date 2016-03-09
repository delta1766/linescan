//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_calib.hpp>

#include <cmath>


namespace linescan{


	widget_calib::widget_calib(camera& cam):
		image_(cam, [this](bool is_live){
			if(is_live){
				extrinsic_button_.setText(tr("Capture"));
			}else{
				extrinsic_button_.setText(tr("Live"));
			}
		})
	{
		std::size_t icon_rows = 100;
		std::size_t icon_cols = static_cast< std::size_t >(
			std::ceil(double(icon_rows) * cam.cols() / cam.rows()));

		extrinsic_images_.setIconSize(QSize(icon_cols, icon_rows));
		tabs_.setMinimumWidth(icon_cols + 50);
		tabs_.setMaximumWidth(icon_cols + 50);

		extrinsics_layout_.addWidget(&extrinsic_images_, 1);
		extrinsics_layout_.addWidget(&extrinsic_button_);
		extrinsics_.setLayout(&extrinsics_layout_);

		tabs_.addItem(&extrinsics_, "Extrinsics");
		tabs_.addItem(&intrinsics_, "Intrinsics");
		tabs_.addItem(&movements_, "Movements");

		layout_.addWidget(&tabs_);
		layout_.addWidget(&image_, 1);

		connect(&extrinsic_button_, &QPushButton::released, [this, icon_rows]{
			if(image_.is_live()){
				image_.stop_live();

				auto image = image_.image();
				image = image.scaledToHeight(icon_rows);
				QIcon icon(QPixmap::fromImage(image));

				new QListWidgetItem(icon, "", &extrinsic_images_);
			}else{
				image_.start_live();
			}
		});

		setLayout(&layout_);
	}


}
