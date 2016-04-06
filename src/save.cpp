//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/save.hpp>

#include <png++/png.hpp>


namespace linescan{


	void save(
		mitrax::raw_bitmap< std::uint8_t > const& image,
		std::string const& name
	){
		png::image< png::gray_pixel > output(image.cols(), image.rows());
		for(std::size_t y = 0; y < image.rows(); ++y){
			for(std::size_t x = 0; x < image.cols(); ++x){
				output[y][x] = image(x, y);
			}
		}
		output.write(name);
	}

	void save(
		mitrax::raw_bitmap< bool > const& image,
		std::string const& name
	){
		png::image< png::packed_gray_pixel< 1 > > output(
			image.cols(),
			image.rows()
		);

		for(std::size_t y = 0; y < image.rows(); ++y){
			for(std::size_t x = 0; x < image.cols(); ++x){
				output[y][x] = image(x, y);
			}
		}

		output.write(name);
	}


}
