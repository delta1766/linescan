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


	inline auto
	erode(mitrax::raw_bitmap< bool > const& image, std::size_t size){
		return transform_per_view([](auto const& m){
			bool result = false;

			for(std::size_t y = 0; y < m.rows(); ++y){
				for(std::size_t x = 0; x < m.cols(); ++x){
					if(!m(x, y)) continue;
					result = true;
				}
			}

			return result;
		}, mitrax::cols(size), mitrax::rows(size), image);
	}



}


#endif
