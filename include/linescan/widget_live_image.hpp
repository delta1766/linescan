//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_live_image__hpp_INCLUDED_
#define _linescan__widget_live_image__hpp_INCLUDED_

#include <linescan/widget_central_image.hpp>
#include <linescan/camera.hpp>


namespace linescan{


	template < typename Image, typename Base >
	class widget_live_image_base: public Base{
	public:
		using processor_type =
			std::function< std::pair< Image, QImage >(
				mitrax::raw_bitmap< std::uint8_t >&&
			) >;

		using live_toggle_callback = std::function< void(bool) >;


		widget_live_image_base(
			camera& cam,
			live_toggle_callback const& c = [](bool){}
		);

		void set_processor(processor_type const& function);

		void reset_processor();

		bool is_live()const;

		void stop_live();

		void start_live();


	protected:
		virtual void showEvent(QShowEvent* event)override;

		virtual void hideEvent(QHideEvent* event)override;


	private:
		void set_live(bool is_live);

		camera& cam_;

		bool is_live_;
		live_toggle_callback live_toggle_callback_;

		processor_type processor_;

		QTimer timer_;
	};

	class widget_live_image: public widget_live_image_base< 
		QImage, widget_central_image
	>{
		using widget_live_image_base< QImage, widget_central_image >::
			widget_live_image_base;
	};

	template < typename T >
	class widget_live_bitmap: public widget_live_image_base< 
		mitrax::raw_bitmap< T >, widget_central_bitmap< T >
	>{
		using widget_live_image_base<
			mitrax::raw_bitmap< T >, widget_central_bitmap< T >
		>::widget_live_image_base;
	};


}


#endif
