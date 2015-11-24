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

#include "vector.hpp"
#include "point.hpp"

#include <mitrax/matrix.hpp>


namespace linescan{


	void draw(mitrax::raw_bitmap< std::uint8_t >& image, point< double > const& point);

	void draw(
		mitrax::raw_bitmap< std::uint8_t >& image,
		vector< point< double > > const& line
	);

	mitrax::raw_bitmap< std::uint8_t > draw_top_distance_line(
		vector< double > const& line,
		std::size_t cols,
		std::size_t rows
	);

	template < typename F >
	inline void draw(mitrax::raw_bitmap< std::uint8_t >& image, F const& fn){
		vector< point< std::decay_t< decltype(fn(std::size_t())) > > >
			line;

		for(std::size_t i = 0; i < image.cols(); ++i){
			auto y = fn(i);
			if(y < 0) continue;
			if(y >= image.rows()) continue;
			line.emplace_back(i, y);
		}

		draw(image, line);
	}


}


#endif
