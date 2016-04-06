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
			mitrax::raw_bitmap< std::uint8_t >&& image
		){
			return { to_image(image), QImage() };
		}


	}


	widget_live_image::widget_live_image(camera& cam):
		cam_(cam),
		is_live_(false),
		processor_(&standard_processor)
	{
		this->connect(&timer_, &QTimer::timeout, [this]{
			exception_catcher([&]{
				auto pair = processor_(cam_.image());

				this->set_images(std::move(pair.first), pair.second);

				if(this->isVisible()) timer_.start(100);
			}, false);
		});
	}

	void widget_live_image::set_processor(
		processor2_type const& function
	){
		processor_ = function;
	}

	void widget_live_image::set_processor(
		processor1_type const& function
	){
		processor_ = [function](bitmap&& image)->std::pair< QImage, QImage >{
			return { function(std::move(image)), QImage() };
		};
	}

	void widget_live_image::reset_processor(){
		processor_ = &standard_processor;
	}

	bool widget_live_image::is_live()const{
		return is_live_;
	}

	void widget_live_image::stop_live(){
		set_live(false);
	}

	void widget_live_image::start_live(){
		set_live(true);
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
		if(is_live){
			timer_.start(0);
		}else{
			timer_.stop();
		}

		is_live_ = is_live;
	}


}
