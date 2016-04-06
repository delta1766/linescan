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


	class widget_live_image: public widget_central_image{
	public:
		using bitmap = mitrax::raw_bitmap< std::uint8_t >;

		using processor1_type = std::function< QImage(bitmap&&) >;
		using processor2_type =
			std::function< std::pair< QImage, QImage >(bitmap&&) >;



		widget_live_image(camera& cam);

		void set_processor(processor1_type const& function);
		void set_processor(processor2_type const& function);

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

		processor2_type processor_;

		QTimer timer_;
	};


}


#endif
