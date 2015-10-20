//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/save_edge.hpp>

#include <linescan/convolution.hpp>
#include <linescan/pixel_wise.hpp>

#include <png++/png.hpp>


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

	bitmap< std::int32_t > amplitude(bitmap< std::uint8_t > const& image){
		auto gx = gradient_x(image);
		auto gy = gradient_y(image);

		return pixel_wise([](auto x, auto y){ return x * x + y * y; }, gx, gy);
	}


	void save_edge(bitmap< std::uint8_t > const& image){
		auto edge = amplitude(image);

		double min = edge(0, 0);
		double max = edge(0, 0);
		pixel_wise([&min, &max](auto v){
			if(min > v) min = v;
			if(max < v) max = v;
		}, edge);

		png::image< png::gray_pixel > output(image.width(), image.height());
		for(std::size_t y = 0; y < edge.height(); ++y){
			for(std::size_t x = 0; x < edge.width(); ++x){
				output[y + 1][x + 1] = static_cast< std::uint8_t >(
					(edge(x, y) - min) / max * 255
				);
			}
		}

		output.write("edge.png");
	}


}
