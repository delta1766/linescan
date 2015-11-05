//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/calc_calibration_lines.hpp>


namespace linescan{


	std::pair< vector< point< float > >, vector< point< float > > >
	calc_calibration_lines(
		vector< float > const& line,
		std::size_t count
	){
		vector< linescan::point< float > > line1;
		vector< linescan::point< float > > line2;
		for(std::size_t i = 0; i < line.size() - count; ++i){
			if(line[i] == 0 || line[i + count] == 0) continue;

			if(line[i] > line[i + count]){
				line1.emplace_back(i, line[i]);
				continue;
			}

			for(i += count; i < line.size(); ++i){
				if(line[i] == 0) continue;
				line2.emplace_back(i, line[i]);
			}
		}

		if(line1.size() > count * 3){
			line1.erase(line1.begin(), line1.begin() + count);
			line1.erase(line1.end() - count, line1.end());
		}else{
			throw std::logic_error(
				"To less points in left laser line part"
			);
		}

		if(line2.size() > count * 3){
			line2.erase(line2.begin(), line2.begin() + count);
			line2.erase(line2.end() - count, line2.end());
		}else{
			throw std::logic_error(
				"To less points in right laser line part"
			);
		}

		return {line1, line2};
	}


}
