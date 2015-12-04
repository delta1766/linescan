//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__invert__hpp_INCLUDED_
#define _linescan__invert__hpp_INCLUDED_

#include "pixel_wise.hpp"


namespace linescan{


	inline auto invert(mitrax::raw_bitmap< bool > const& image){
		return make_matrix_pixel_wise([](auto v){
			return !v;
		}, image);
	}


}


#endif
