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


	widget_live_image::widget_live_image(
		camera& cam,
		live_toggle_callback const& callback
	):
		cam_(cam),
		is_live_(false),
		live_toggle_callback_(callback),
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

	bool widget_live_image::is_live()const{
		return is_live_;
	}

	void widget_live_image::stop_live(){
		set_live(false);
		timer_.stop();
	}

	void widget_live_image::start_live(){
		set_live(true);
		timer_.start(0);
	}

	void widget_live_image::showEvent(QShowEvent* event){
		QWidget::showEvent(event);
		start_live();
	}

	void widget_live_image::hideEvent(QHideEvent* event){
		QWidget::hideEvent(event);
		stop_live();
	}

	void widget_live_image::set_live(bool is_live){
		is_live_ = is_live;
		if(live_toggle_callback_) live_toggle_callback_(is_live_);
	}


}
