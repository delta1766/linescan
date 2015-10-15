//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/calc_line.hpp>

#include <linescan/convolution.hpp>

#include <png++/png.hpp>


namespace linescan{


	std::vector< float > calc_line(bitmap< std::uint8_t > image){
		std::vector< float > result(image.width());

		for(std::size_t x = 0; x < image.width(); ++x){
			std::size_t max_length = 0;
			std::size_t start = 0;
			for(std::size_t y = 1; y < image.height(); ++y){
				if(image(x, y) < 255){
					if(start == 0) continue;

					auto length = y - start;
					if(length > max_length){
						max_length = length;

						result[x] = start + length / 2.f;
					}

					start = 0;
				}else{
					if(start != 0) continue;

					start = y;
				}
			}
		}

// 		png::image< png::gray_pixel > output(image.width(), image.height());
// 		for(std::size_t y = 0; y < image.height(); ++y){
// 			for(std::size_t x = 0; x < image.width(); ++x){
// 				output[y][x] = image(x, y) < 255 ? 0 : 255;
// 			}
// 		}
// 		output.write("binary.png");
// 		for(std::size_t y = 0; y < image.height(); ++y){
// 			for(std::size_t x = 0; x < image.width(); ++x){
// 				output[y][x] = 0;
// 			}
// 		}
// 
// 		for(std::size_t x = 0; x < image.width(); ++x){
// 			auto pos1 = static_cast< std::size_t >(result[x]);
// 			auto pos2 = static_cast< std::size_t >(result[x] - 0.5f);
// 			if(pos1 == pos2){
// 				output[pos1][x] = 255;
// 			}else{
// 				output[pos1][x] = 128;
// 				output[pos2][x] = 128;
// 			}
// 		}
// 		output.write("line.png");

		return result;
	}


}
