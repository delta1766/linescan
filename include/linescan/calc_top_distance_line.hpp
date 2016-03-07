//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__calc_top_distance_line__hpp_INCLUDED_
#define _linescan__calc_top_distance_line__hpp_INCLUDED_

#include "vector.hpp"

#include <mitrax/matrix.hpp>


namespace linescan{


	vector< double > calc_top_distance_line(
		mitrax::raw_bitmap< std::uint8_t > const& image
	);

	vector< double > calc_top_distance_line(
		mitrax::raw_bitmap< bool > const& image
	);


}


#endif
