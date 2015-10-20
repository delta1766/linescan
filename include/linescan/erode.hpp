//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__erode__hpp_INCLUDED_
#define _linescan__erode__hpp_INCLUDED_

#include "pixel_wise.hpp"


namespace linescan{


	inline auto erode(bitmap< bool > const& image, std::size_t size){
		return offset_view_wise([size](auto view){
			bool result = false;

			for(std::size_t y = 0; y < size; ++y){
				for(std::size_t x = 0; x < size; ++x){
					if(!view(x, y)) continue;
					result = true;
				}
			}

			return result;
		}, size, size, image);
	}



}


#endif
