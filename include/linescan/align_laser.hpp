//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__align_laser__hpp_INCLUDED_
#define _linescan__align_laser__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <QtGui/QPixmap>


namespace linescan{


	QPixmap to_pixmap(mitrax::raw_bitmap< std::uint8_t > const& bitmap);

	std::tuple< QString, QPixmap > align_laser();


}


#endif
