//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__circlefind__hpp_INCLUDED_
#define _linescan__circlefind__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <vector>


namespace linescan{


	std::vector< double > circlefind(
		mitrax::raw_bitmap< std::uint8_t > const& image,
		std::size_t x_count, std::size_t y_count,
		float radius_in_mm, float distance_in_mm
	);


}


#endif
