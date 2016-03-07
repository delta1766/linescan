//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__calib_laser__hpp_INCLUDED_
#define _linescan__calib_laser__hpp_INCLUDED_

#include "point.hpp"
#include "camera.hpp"

#include <QtGui/QImage>

#include <vector>


namespace linescan{


	QImage calib_laser_pixmap(
		camera& cam,
		std::vector< point< double > > const& points
	);


}


#endif
