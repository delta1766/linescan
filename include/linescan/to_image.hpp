//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__to_image__hpp_INCLUDED_
#define _linescan__to_image__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <QImage>


namespace linescan{


	/// \brief Convert to QImage
	QImage to_image(mitrax::std_bitmap< std::uint8_t > const& bitmap);

	/// \brief Convert to QImage
	QImage to_image(mitrax::std_bitmap< bool > const& bitmap);


}


#endif
