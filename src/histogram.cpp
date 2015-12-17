//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/histogram.hpp>


namespace linescan{


	mitrax::raw_bitmap< std::uint8_t > histogram(
		mitrax::raw_bitmap< std::uint8_t > const& image
	){
		std::size_t histogram[256] = {0};
		for(std::size_t y = 0; y < image.rows(); ++y){
			for(std::size_t x = 0; x < image.cols(); ++x){
				++histogram[image(x, y)];
			}
		}

		std::size_t max = 0;
		for(auto v: histogram) if(max < v) max = v;

		auto output = mitrax::make_matrix< std::uint8_t >(
			mitrax::cols(256),
			mitrax::rows(max / 256)
		);

		for(std::size_t x = 0; x < 256; ++x){
			for(std::size_t y = 0; y < histogram[x]; ++y){
				output(x, y) =
					histogram[x] / 256 == 0 ? histogram[x] % 256 : 255;
			}
		}

		return output;
	}


}
