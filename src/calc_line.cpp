//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/calc_line.hpp>

#include <linescan/pixel_wise.hpp>

#include <png++/png.hpp>


namespace linescan{


	auto binarization(
		std::uint8_t threshold,
		bitmap< std::uint8_t > const& image
	){
		return pixel_wise([threshold](auto v){ return v < threshold; }, image);
	}

	auto erode(std::size_t size, bitmap< bool > image){
		return offset_view_wise([size](auto view){
			bool result = false;

			for(std::size_t y = 0; y < size; ++y){
				for(std::size_t x = 0; x < size; ++x){
					if(!view(x, y)) continue;
					result = true;
				}
			}

			return result;
		}, size, size, image);
	}

	std::vector< float > calc_line(bitmap< std::uint8_t > const& image){
		auto binary = binarization(255, image);
		binary = erode(5, binary);

		std::vector< float > result(binary.width());
		for(std::size_t x = 0; x < binary.width(); ++x){
			std::size_t max_length = 0;
			std::size_t start = 0;
			for(std::size_t y = 1; y < binary.height(); ++y){
				if(!binary(x, y)){
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

		return result;
	}


}
