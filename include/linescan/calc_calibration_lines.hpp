//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__calc_calibration_lines__hpp_INCLUDED_
#define _linescan__calc_calibration_lines__hpp_INCLUDED_

#include "point.hpp"
#include "vector.hpp"

#include <utility>


namespace linescan{


	std::pair< vector< point< double > >, vector< point< double > > >
	calc_calibration_lines(
		vector< double > const& top_distance_line,
		std::size_t count
	);


}


#endif
