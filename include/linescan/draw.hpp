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

#include <cmath>


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

	template < typename T >
	inline void draw_cycle(
		mitrax::raw_bitmap< std::uint8_t >& image,
		mitrax::raw_col_vector< T, 2 > const& p,
		T const& radius
  	){
		vector< point< T > > points;

		auto c = 2 * 3.14159 * radius;
		auto count = static_cast< size_t >(c) + 1;
		for(std::size_t i = 0; i < count; ++i){
			auto pos = 2 * 3.14159 / count * i;
			auto dx = std::sin(pos);
			auto dy = std::cos(pos);
			points.emplace_back(
				p[0] - dx * radius,
				p[1] - dy * radius
			);
		}

		draw(image, points);
	}

	template < typename T >
	inline void draw_cycle(
		mitrax::raw_bitmap< std::uint8_t >& image,
		point< T > const& p,
		T const& radius
  	){
		using namespace mitrax;
		using namespace mitrax::literals;
		draw_cycle(image, make_col_vector< T >(2_R, {p.x(), p.y()}), radius);
	}


}


#endif
