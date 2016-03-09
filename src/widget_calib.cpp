//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_calib.hpp>


namespace linescan{


	widget_calib::widget_calib(camera& cam):
		image_(cam),
		extrinsic_button_(tr("Capture"))
	{
		extrinsics_layout_.addWidget(&extrinsic_images_, 1);
		extrinsics_layout_.addWidget(&extrinsic_button_);
		extrinsics_.setLayout(&extrinsics_layout_);

		tabs_.addItem(&extrinsics_, "Extrinsics");
		tabs_.addItem(&intrinsics_, "Intrinsics");
		tabs_.addItem(&movements_, "Movements");

		layout_.addWidget(&tabs_);
		layout_.addWidget(&image_, 1);
		setLayout(&layout_);
	}


}
