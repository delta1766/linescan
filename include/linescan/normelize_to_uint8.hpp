//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__normelize_to_uint8__hpp_INCLUDED_
#define _linescan__normelize_to_uint8__hpp_INCLUDED_

#include "pixel_wise.hpp"


namespace linescan{


	template < typename T >
	inline mitrax::raw_bitmap< std::uint8_t >
	normelize_to_uint8(mitrax::raw_bitmap< T > const& image){
		auto min = image(0, 0);
		auto max = image(0, 0);
		for_each([&min, &max](auto v){
			if(min > v){
				min = v;
			}else if(max < v){
				max = v;
			}
		}, image);

		return transform([&min, &max](auto v){
			auto r = (static_cast< long double >(v) - min) / max * 255;

			return static_cast< std::uint8_t >(r < 0 ? 0 : r > 255 ? 255 : r);
		}, image);
	}



}


#endif
