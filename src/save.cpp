//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/save.hpp>

#include <png++/png.hpp>

#include <iostream>


namespace linescan{


	void save(bitmap< std::uint8_t > const& image, std::string const& name){
		std::cout << "write " << name << std::endl;

		png::image< png::gray_pixel > output(image.width(), image.height());
		for(std::size_t y = 0; y < image.height(); ++y){
			for(std::size_t x = 0; x < image.width(); ++x){
				output[y][x] = image(x, y);
			}
		}
		output.write(name);
	}

	void save(bitmap< bool > const& image, std::string const& name){
		std::cout << "write " << name << std::endl;

		png::image< png::packed_gray_pixel< 1 > > output(
			image.width(),
			image.height()
		);

		for(std::size_t y = 0; y < image.height(); ++y){
			for(std::size_t x = 0; x < image.width(); ++x){
				output[y][x] = image(x, y);
			}
		}

		output.write(name);
	}


}
