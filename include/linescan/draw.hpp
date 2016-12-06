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

#include "polynom.hpp"
#include "circle.hpp"

#include <QImage>


namespace linescan{


	/// \brief Draw a white subpixel point on image
	void draw(
		mitrax::std_bitmap< std::uint8_t >& image,
		mitrax::point< double > const& point
	);

	/// \brief Draw white subpixel points on image
	void draw(
		mitrax::std_bitmap< std::uint8_t >& image,
		std::vector< mitrax::point< double > > const& line
	);

	/// \brief Create an black image and draw white subpixel points on it
	mitrax::std_bitmap< std::uint8_t > draw_laser_line(
		std::vector< mitrax::point< double > > const& line,
		std::size_t cols,
		std::size_t rows
	);

	/// \brief Fit a linear function over the points and draw it and its angle
	QImage draw_laser_alignment(
		mitrax::rt_dim_pair_t const& dims,
		std::vector< mitrax::point< double > > const& line
	);

	/// \brief Find the two calibration circles and draw the linear function
	///        between them
	QImage draw_circle_line(
		mitrax::std_bitmap< std::uint8_t > const& bitmap
	);

	/// \brief Create a linear function beween the two circles and draw it and
	///        its angle
	QImage draw_circle_line(
		mitrax::std_bitmap< std::uint8_t > const& bitmap,
		std::vector< circle > const& circles
	);

	/// \brief Draw the linear function and its angle to the x-axis
	QImage draw_line(
		mitrax::rt_dim_pair_t const& dims,
		polynom< double, 1 > const& line
	);

	/// \brief Draw the linear function and its angle to the x-axis
	void draw_line(QImage& overlay, polynom< double, 1 > const& line);


}


#endif
