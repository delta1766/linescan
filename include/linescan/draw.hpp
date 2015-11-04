//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__draw__hpp_INCLUDED_
#define _linescan__draw__hpp_INCLUDED_

#include "bitmap.hpp"

#include <vector>


namespace linescan{


	void draw(bitmap< std::uint8_t >& image, point< float > const& point);

	void draw(
		bitmap< std::uint8_t >& image,
		std::vector< point< float > > const& line
	);

	bitmap< std::uint8_t > draw_top_distance_line(
		std::vector< float > const& line,
		std::size_t width,
		std::size_t height
	);

	template < typename F >
	inline void draw(bitmap< std::uint8_t >& image, F const& fn){
		std::vector< point< std::decay_t< decltype(fn(std::size_t())) > > >
			line;

		for(std::size_t i = 0; i < image.width(); ++i){
			auto y = fn(i);
			if(y < 0) continue;
			if(y >= image.height()) continue;
			line.emplace_back(i, y);
		}

		draw(image, line);
	}


}


#endif
