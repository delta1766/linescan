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

#include <mitrax/point.hpp>

#include <array>
#include <vector>


namespace linescan{


	std::array< std::vector< mitrax::point< double > >, 2 >
	calc_calibration_lines(
		std::vector< double > const& top_distance_line,
		std::size_t count
	);


}


#endif
