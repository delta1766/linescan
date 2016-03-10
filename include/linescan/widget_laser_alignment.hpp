//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_laser_alignment__hpp_INCLUDED_
#define _linescan__widget_laser_alignment__hpp_INCLUDED_

#include "widget_live_image.hpp"

#include <QtWidgets/QtWidgets>


namespace linescan{


	class widget_laser_alignment: public QWidget{
	public:
		widget_laser_alignment(camera& cam);


	private:
		QHBoxLayout layout_;
		widget_live_bitmap< std::uint8_t > image_;
	};


}


#endif
