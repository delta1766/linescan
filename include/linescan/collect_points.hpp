//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__collect_points__hpp_INCLUDED_
#define _linescan__collect_points__hpp_INCLUDED_

#include "vector.hpp"
#include "point.hpp"

#include <mitrax/matrix.hpp>

#include <utility>


namespace linescan{


	vector< std::pair< point< double >, std::size_t > >
	collect_points(mitrax::raw_bitmap< bool > const& image);


}


#endif