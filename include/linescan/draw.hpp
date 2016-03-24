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

#include "processing.hpp"
#include "polynom.hpp"
#include "circlefind.hpp"

#include <mitrax/matrix.hpp>

#include <QtGui/QImage>

#include <cmath>
#include <vector>


namespace linescan{


	void draw(
		mitrax::raw_bitmap< std::uint8_t >& image,
		mitrax::point< double > const& point
	);

	void draw(
		mitrax::raw_bitmap< std::uint8_t >& image,
		std::vector< mitrax::point< double > > const& line
	);

	mitrax::raw_bitmap< std::uint8_t > draw_top_distance_line(
		std::vector< mitrax::point< double > > const& line,
		std::size_t cols,
		std::size_t rows
	);

	mitrax::raw_bitmap< bool > draw_top_distance_line_student(
		std::vector< mitrax::point< double > > const& line,
		std::size_t cols,
		std::size_t rows
	);

	QImage draw_laser_alignment(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		std::uint8_t binarize_threshold,
		std::size_t erode_value
	);

	QImage draw_laser_alignment(
		mitrax::bitmap_dims_t const& dims,
		std::vector< mitrax::point< double > > const& line
	);

	QImage draw_circle_line(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap
	);

	QImage draw_circle_line(
		mitrax::bitmap_dims_t const& dims,
		std::array< circle, 2 > const& circles
	);

	QImage draw_line(
		mitrax::bitmap_dims_t const& dims,
		polynom< double, 1 > const& line
	);


}


#endif
