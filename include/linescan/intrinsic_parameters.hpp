//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__intrinsic_parameters__hpp_INCLUDED_
#define _linescan__intrinsic_parameters__hpp_INCLUDED_

#include "camera.hpp"
#include "point.hpp"

#include <vector>
#include <array>


namespace linescan{


	std::vector< point< float > > find_chessboard_corners(camera& cam);

	std::array< double, 3 > calc_intrinsic_parameters(
		camera& cam,
		std::vector< std::vector< point< float > > > const& ref_points
	);


}


#endif
