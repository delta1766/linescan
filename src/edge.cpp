//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/edge.hpp>

#include <linescan/convolution.hpp>
#include <linescan/pixel_wise.hpp>


namespace linescan{


	bitmap< std::int16_t > gradient_x(bitmap< std::uint8_t > const& image){
		return convolution< std::int16_t >(
			image,
			matrix< std::int16_t, 1, 3 >{1, 0, -1},
			matrix< std::int16_t, 3, 1 >{3, 10, 3}
		);
	}

	bitmap< std::int16_t > gradient_y(bitmap< std::uint8_t > const& image){
		return convolution< std::int16_t >(
			image,
			matrix< std::int16_t, 1, 3 >{3, 10, 3},
			matrix< std::int16_t, 3, 1 >{1, 0, -1}
		);
	}

	bitmap< std::int32_t > edge_amplitude(bitmap< std::uint8_t > const& image){
		auto gx = gradient_x(image);
		auto gy = gradient_y(image);

		return pixel_wise([](std::int32_t x, std::int32_t y){
			return x * x + y * y;
		}, gx, gy);
	}


}
