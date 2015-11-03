//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__binarize__hpp_INCLUDED_
#define _linescan__binarize__hpp_INCLUDED_

#include "pixel_wise.hpp"


namespace linescan{


	template < typename T >
	inline auto binarize(bitmap< T > const& image, T const& threshold){
		return pixel_wise([threshold](auto v){
			return v >= threshold;
		}, image);
	}


}


#endif
