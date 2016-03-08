//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_live_image.hpp>
#include <linescan/exception_catcher.hpp>
#include <linescan/to_image.hpp>


namespace linescan{


	namespace{

		std::pair< QImage, QImage > standard_processor(
			mitrax::raw_bitmap< std::uint8_t > const& bitmap
		){
			return { to_image(bitmap), QImage() };
		}

	}

	widget_live_image::widget_live_image(camera& cam):
		cam_(cam),
		processor_(&standard_processor)
	{
		connect(&timer_, &QTimer::timeout, [this]{
			exception_catcher([&]{
				QImage image, overlay;
				std::tie(image, overlay) = processor_(cam_.image());

				set_images(image, overlay);

				if(isVisible()) timer_.start(100);
			});
		});
	}

	void widget_live_image::set_processor(processor_type const& function){
		processor_ = function;
	}

	void widget_live_image::reset_processor(){
		processor_ = &standard_processor;
	}

	void widget_live_image::showEvent(QShowEvent* event){
		QWidget::showEvent(event);
		timer_.start(0);
	}

	void widget_live_image::hideEvent(QHideEvent* event){
		QWidget::hideEvent(event);
		timer_.stop();
	}


}
