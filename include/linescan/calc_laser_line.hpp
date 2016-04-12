//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__calc_laser_line__hpp_INCLUDED_
#define _linescan__calc_laser_line__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <QImage>

#include <vector>


namespace linescan{


	struct calc_laser_line_mode{
		enum class threshold{
			original,
			binarize,
			erode,
			line
		};

		enum class sum{
			original,
			line
		};
	};

	struct as_image_t{};
	constexpr auto as_image = as_image_t();

	struct points_and_image_t{};
	constexpr auto points_and_image = points_and_image_t();

	class calc_laser_line_t{
	public:
		std::vector< mitrax::point< double > >
		operator()(mitrax::raw_bitmap< std::uint8_t > const& image)const;

		QImage operator()(
			mitrax::raw_bitmap< std::uint8_t > const& image,
			as_image_t
		)const;

		std::pair< std::vector< mitrax::point< double > >, QImage >
		operator()(
			mitrax::raw_bitmap< std::uint8_t > const& image,
			points_and_image_t
		)const;

		void use(
			calc_laser_line_mode::threshold mode,
			std::uint8_t threshold,
			std::size_t erode,
			bool subpixel
		);

		void use(
			calc_laser_line_mode::sum mode,
			std::uint8_t min_value,
			std::size_t min_sum,
			bool subpixel
		);

	private:
		enum class type{
			threshold,
			sum
		};

		type method_ = type::threshold;

		calc_laser_line_mode::threshold threshold_mode_;
		std::uint8_t threshold_ = 255;
		std::size_t erode_ = 2;
		bool threshold_subpixel_ = true;

		calc_laser_line_mode::sum sum_mode_;
		std::uint8_t min_value_ = 20;
		std::size_t min_sum_ = 4000;
		bool sum_subpixel_ = true;
	};

	extern calc_laser_line_t calc_laser_line;


}


#endif
