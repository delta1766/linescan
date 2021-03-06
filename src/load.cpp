//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/load.hpp>

#include <png++/png.hpp>


namespace linescan{


	mitrax::std_bitmap< std::uint8_t > load(std::string const& name){
		png::image< png::gray_pixel > output;
		output.read(name);
		auto image = mitrax::make_matrix_v< std::uint8_t >(
			mitrax::cols(output.get_width()), mitrax::rows(output.get_height())
		);

		for(std::size_t y = 0; y < output.get_height(); ++y){
			for(std::size_t x = 0; x < output.get_width(); ++x){
				image(x, y) = output[y][x];
			}
		}

		return image;
	}


}
