//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/calc_top_distance_line.hpp>

#include <linescan/binarize.hpp>
#include <linescan/erode.hpp>


namespace linescan{


	vector< double > calc_top_distance_line(
		mitrax::raw_bitmap< bool > const& binary
	){
		vector< double > result(binary.cols());
		for(std::size_t x = 0; x < binary.cols(); ++x){
			std::size_t max_length = 0;
			std::size_t start = 0;
			for(std::size_t y = 1; y < binary.rows(); ++y){
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
