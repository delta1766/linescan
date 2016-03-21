//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_laser_alignment.hpp>
#include <linescan/draw.hpp>


namespace linescan{


	widget_laser_alignment::widget_laser_alignment(camera& cam):
		image_(cam)
	{
		image_.set_processor(&draw_laser_alignment);

		layout_.addWidget(&image_);
		setLayout(&layout_);
	}


}
