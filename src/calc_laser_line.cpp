//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/calc_laser_line.hpp>

#include <linescan/processing.hpp>


namespace linescan{


	std::vector< mitrax::point< double > > calc_laser_line(
		mitrax::raw_bitmap< bool > const& binary
	){
		std::vector< mitrax::point< double > > result;
		result.reserve(binary.cols());
		for(std::size_t x = 0; x < binary.cols(); ++x){
			std::size_t max_length = 0;
			std::size_t start = 0;
			for(std::size_t y = 1; y < binary.rows(); ++y){
				if(!binary(x, y)){
					if(start == 0) continue;

					auto length = y - start;
					if(length > max_length){
						max_length = length;

						result.emplace_back(x, start + length / 2.);
					}

					start = 0;
				}else{
					if(start != 0) continue;

					start = y;
				}
			}
		}
		result.shrink_to_fit();

		return result;
	}

	std::vector< mitrax::point< double > > calc_laser_line(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		std::uint8_t binarize_threshold,
		std::size_t erode_value
	){
		auto binary = binarize(bitmap, binarize_threshold);
		binary = erode(binary, erode_value);
		return calc_laser_line(binary);
	}

	std::vector< mitrax::point< double > > calc_laser_line_via_sum(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		std::uint8_t min,
		std::size_t min_sum
	){
		std::vector< mitrax::point< double > > result;
		result.reserve(bitmap.cols());
		for(std::size_t x = 0; x < bitmap.cols(); ++x){
			std::size_t sum = 0;
			for(std::size_t y = 0; y < bitmap.rows(); ++y){
				auto v = bitmap(x, y);
				if(v <= min) continue;
				sum += v - min;
			}

			if(sum < min_sum) continue;

			std::size_t threshold = 0;
			std::size_t index = 0;
			for(std::size_t y = 0; y < bitmap.rows(); ++y){
				auto v = bitmap(x, y);
				if(v <= min) continue;
				threshold += v - min;
				if(threshold > sum / 2.){
					index = y;
					break;
				}
			}

			auto y = index + bitmap(x, index) / (threshold - sum / 2.);

			result.emplace_back(x, y);
		}
		result.shrink_to_fit();

		return result;
	}


}
