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
#include <linescan/binarize.hpp>
#include <linescan/to_image.hpp>


namespace linescan{


	namespace{


		template < typename Image >
		struct convert;

		template <>
		struct convert< mitrax::raw_bitmap< std::uint8_t > >{
			static mitrax::raw_bitmap< std::uint8_t >
			f(mitrax::raw_bitmap< std::uint8_t >&& image){
				return std::move(image);
			}
		};

		template <>
		struct convert< mitrax::raw_bitmap< bool > >{
			static mitrax::raw_bitmap< bool >
			f(mitrax::raw_bitmap< std::uint8_t >&& image){
				return binarize(image, std::uint8_t(127));
			}
		};

		template <>
		struct convert< QImage >{
			static QImage
			f(mitrax::raw_bitmap< std::uint8_t >&& image){
				return to_image(std::move(image));
			}
		};

		template < typename Image >
		std::pair< Image, QImage > standard_processor(
			mitrax::raw_bitmap< std::uint8_t >&& image
		){
			return { convert< Image >::f(std::move(image)), QImage() };
		}


	}


	template < typename Image, typename Base >
	widget_live_image_base< Image, Base >::widget_live_image_base(
		camera& cam,
		live_toggle_callback const& callback
	):
		cam_(cam),
		is_live_(false),
		live_toggle_callback_(callback),
		processor_(&standard_processor< Image >)
	{
		this->connect(&timer_, &QTimer::timeout, [this]{
			exception_catcher([&]{
				auto pair = processor_(cam_.image());

				this->set_images(std::move(pair.first), pair.second);

				if(this->isVisible()) timer_.start(100);
			});
		});
	}

	template < typename Image, typename Base >
	void widget_live_image_base< Image, Base >::set_processor(
		processor_type const& function
	){
		processor_ = function;
	}

	template < typename Image, typename Base >
	void widget_live_image_base< Image, Base >::reset_processor(){
		processor_ = &standard_processor< Image >;
	}

	template < typename Image, typename Base >
	bool widget_live_image_base< Image, Base >::is_live()const{
		return is_live_;
	}

	template < typename Image, typename Base >
	void widget_live_image_base< Image, Base >::stop_live(){
		set_live(false);
		timer_.stop();
	}

	template < typename Image, typename Base >
	void widget_live_image_base< Image, Base >::start_live(){
		set_live(true);
		timer_.start(0);
	}

	template < typename Image, typename Base >
	void widget_live_image_base< Image, Base >::showEvent(QShowEvent* event){
		QWidget::showEvent(event);
		start_live();
	}

	template < typename Image, typename Base >
	void widget_live_image_base< Image, Base >::hideEvent(QHideEvent* event){
		QWidget::hideEvent(event);
		stop_live();
	}

	template < typename Image, typename Base >
	void widget_live_image_base< Image, Base >::set_live(bool is_live){
		is_live_ = is_live;
		if(live_toggle_callback_) live_toggle_callback_(is_live_);
	}


	template class widget_live_image_base<
		QImage,
		widget_central_image
	>;

	template class widget_live_image_base<
		mitrax::raw_bitmap< bool >,
		widget_central_bitmap< bool >
	>;

	template class widget_live_image_base<
		mitrax::raw_bitmap< std::uint8_t >,
		widget_central_bitmap< std::uint8_t >
	>;


}
