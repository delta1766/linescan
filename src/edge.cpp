//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/edge.hpp>

#include <mitrax/transform.hpp>
#include <mitrax/convolution.hpp>


namespace linescan{


	auto gradient_x(
		mitrax::raw_bitmap< std::uint8_t > const& image
	){
		using namespace mitrax::literals;
		return convolution(
			image,
			mitrax::make_col_vector< std::int16_t >(3_R, {3, 10, 3}),
			mitrax::make_row_vector< std::int16_t >(3_C, {1, 0, -1})
		);
	}

	auto gradient_y(
		mitrax::raw_bitmap< std::uint8_t > const& image
	){
		using namespace mitrax::literals;
		return convolution(
			image,
			mitrax::make_col_vector< std::int16_t >(3_R, {1, 0, -1}),
			mitrax::make_row_vector< std::int16_t >(3_C, {3, 10, 3})
		);
	}

	mitrax::raw_bitmap< std::int32_t > edge_amplitude(
		mitrax::raw_bitmap< std::uint8_t > const& image
	){
		auto gx = gradient_x(image);
		auto gy = gradient_y(image);

		return mitrax::transform([](std::int32_t x, std::int32_t y){
			return x * x + y * y;
		}, gx, gy);
	}


}
