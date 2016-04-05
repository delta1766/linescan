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

#include <vector>


namespace linescan{


	class calc_laser_line_t{
	public:
		std::vector< mitrax::point< double > > operator()(
			mitrax::raw_bitmap< std::uint8_t > const& image
		)const;

		void use_threshold(std::uint8_t threshold, std::size_t erode);
		void use_sum(std::uint8_t min_value, std::size_t min_sum);

	private:
		enum class type{
			threshold,
			sum
		};

		type method_ = type::threshold;

		std::uint8_t threshold_ = 255;
		std::size_t erode_ = 2;

		std::uint8_t min_value_ = 20;
		std::size_t min_sum_ = 4000;
	};

	extern calc_laser_line_t calc_laser_line;


}


#endif
