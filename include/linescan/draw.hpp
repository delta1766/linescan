//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
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

#include <QtGui/QImage>

#include <cmath>


namespace linescan{


	void draw(
		mitrax::raw_bitmap< std::uint8_t >& image,
		point< double > const& point
	);

	void draw(
		mitrax::raw_bitmap< std::uint8_t >& image,
		vector< point< double > > const& line
	);

	mitrax::raw_bitmap< std::uint8_t > draw_top_distance_line(
		vector< double > const& line,
		std::size_t cols,
		std::size_t rows
	);

	mitrax::raw_bitmap< bool > draw_top_distance_line_student(
		vector< double > const& line,
		std::size_t cols,
		std::size_t rows
	);

	std::pair< QImage, QImage >
	draw_laser_alignment(mitrax::raw_bitmap< std::uint8_t >&& bitmap);


}


#endif
