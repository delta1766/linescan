//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/histogram.hpp>

#include <linescan/convolution.hpp>


namespace linescan{


	bitmap< std::uint8_t > histogram(bitmap< std::uint8_t > const& image){
		std::size_t histogram[256] = {0};
		for(std::size_t y = 0; y < image.height(); ++y){
			for(std::size_t x = 0; x < image.width(); ++x){
				++histogram[image(x, y)];
			}
		}

		std::size_t max = 0;
		for(auto v: histogram) if(max < v) max = v;

		bitmap< std::uint8_t > output(256, max / 256);
		for(std::size_t x = 0; x < 256; ++x){
			for(std::size_t y = 0; y < histogram[x]; ++y){
				output(x, y) =
					histogram[x] / 256 == 0 ? histogram[x] % 256 : 255;
			}
		}

		return output;
	}


}
