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

#include "bitmap.hpp"

#include <utility>


namespace linescan{


	vector< std::pair< point< float >, std::size_t > >
	collect_points(bitmap< bool > const& image);


}


#endif
