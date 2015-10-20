//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/calc_line.hpp>

#include <linescan/binarize.hpp>
#include <linescan/erode.hpp>

#include <png++/png.hpp>


namespace linescan{


	std::vector< float > calc_line(bitmap< std::uint8_t > const& image){
		auto binary = binarize(image, std::uint8_t(255));
		binary = erode(binary, 5);

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
