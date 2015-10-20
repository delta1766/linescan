//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__pixel_wise__hpp_INCLUDED_
#define _linescan__pixel_wise__hpp_INCLUDED_

#include "bitmap.hpp"


namespace linescan{


	template < typename T, typename F >
	inline auto pixel_wise(bitmap< T > const& image, F const& f)
		-> bitmap< decltype(f(std::declval< T >())) >
	{
		bitmap< decltype(f(std::declval< T >())) > result(image.size());

		for(std::size_t y = 0; y < image.height(); ++y){
			for(std::size_t x = 0; x < image.width(); ++x){
				result(x, y) = f(image(x, y));
			}
		}

		return result;
	}


}


#endif
