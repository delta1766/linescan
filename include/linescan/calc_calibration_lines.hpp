//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__calc_calibration_lines__hpp_INCLUDED_
#define _linescan__calc_calibration_lines__hpp_INCLUDED_

#include "point.hpp"

#include <vector>
#include <utility>


namespace linescan{


	std::pair< std::vector< point< float > >, std::vector< point< float > > >
	calc_calibration_lines(
		std::vector< float > const& top_distance_line,
		std::size_t count
	);


}


#endif
