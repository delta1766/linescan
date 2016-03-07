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
		using processor_type = std::function<
			std::pair< QImage, QImage >(
				mitrax::raw_bitmap< std::uint8_t > const&
			)
		>;

		widget_live_image(camera& cam);

		void set_processor(processor_type const& function);

		void reset_processor();


	protected:
		virtual void showEvent(QShowEvent* event)override;

		virtual void hideEvent(QHideEvent* event)override;


	private:
		camera& cam_;

		processor_type processor_;

		QTimer timer_;
	};


}


#endif
