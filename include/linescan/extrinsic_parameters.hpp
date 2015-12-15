//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__extrinsic_parameters__hpp_INCLUDED_
#define _linescan__extrinsic_parameters__hpp_INCLUDED_

#include "camera.hpp"
#include "point.hpp"

#include <array>


namespace linescan{


	std::array< point< double >, 8 > find_ref_points(camera& cam);


	std::tuple< std::array< double, 3 >, std::array< double, 3 > >
	calc_extrinsic_parameters(
		std::array< double, 3 > const& camera_matrix_parameter,
		std::array< double, 8 > const& distortion_coefficients,
		std::array< point< double >, 8 > const& points
	);


}


#endif
