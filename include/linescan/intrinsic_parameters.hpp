//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__intrinsic_parameters__hpp_INCLUDED_
#define _linescan__intrinsic_parameters__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <vector>
#include <array>


namespace linescan{


	std::vector< mitrax::point< float > > find_chessboard_corners(
		mitrax::raw_bitmap< std::uint8_t > const& image
	);

	std::tuple< std::array< double, 3 >, std::array< double, 8 > >
	calc_intrinsic_parameters(
		mitrax::bitmap_dims_t const& dims,
		std::vector< std::vector< mitrax::point< float > > > const& ref_points
	);


}


#endif
