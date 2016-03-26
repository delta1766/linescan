//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__calc_laser_line__hpp_INCLUDED_
#define _linescan__calc_laser_line__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <vector>


namespace linescan{


	std::vector< mitrax::point< double > > calc_laser_line(
		mitrax::raw_bitmap< bool > const& image
	);

	std::vector< mitrax::point< double > > calc_laser_line(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		std::uint8_t binarize_threshold,
		std::size_t erode_value
	);


}


#endif
