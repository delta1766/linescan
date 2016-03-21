//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_laser_alignment.hpp>
#include <linescan/to_image.hpp>
#include <linescan/draw.hpp>


namespace linescan{


	widget_laser_alignment::widget_laser_alignment(camera& cam):
		image_(cam)
	{
		image_.set_processor(
			[this](mitrax::raw_bitmap< std::uint8_t >&& bitmap){
				auto overlay = draw_laser_alignment(bitmap, 255, 3);
				return std::pair< QImage, QImage >(
					to_image(std::move(bitmap)), overlay
				);
			});

		layout_.addWidget(&image_);
		setLayout(&layout_);
	}


}
