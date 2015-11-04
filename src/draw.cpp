//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/draw.hpp>


namespace linescan{


	void draw(bitmap< std::uint8_t >& image, point< float > const& point){
		auto x = static_cast< int >(point.x());
		auto y = static_cast< int >(point.y());

		auto dx = point.x() - x;
		auto dy = point.y() - y;

		auto draw = [&image](int x, int y, float v){
			if(
				x < 0 || y < 0 ||
				x >= static_cast< int >(image.width()) ||
				y >= static_cast< int >(image.height())
			) return;

			auto r = image(x, y) + v;
			image(x, y) = static_cast< std::uint8_t >(r > 255 ? 255 : r);
		};

		draw(x, y, 255 * (1 - dx) * (1 - dy));
		draw(x + 1, y, 255 * dx * (1 - dy));
		draw(x, y + 1, 255 * (1 - dx) * dy);
		draw(x + 1, y + 1, 255 * dx * dy);
	}

	void draw(
		bitmap< std::uint8_t >& image,
		std::vector< point< float > > const& line
	){
		for(std::size_t i = 0; i < line.size(); ++i){
			draw(image, line[i]);
		}
	}

	bitmap< std::uint8_t > draw_top_distance_line(
		std::vector< float > const& line,
		std::size_t width,
		std::size_t height
	){
		bitmap< std::uint8_t > image(width, height);

		std::vector< point< float > > point_line;
		for(std::size_t i = 0; i < line.size(); ++i){
			if(line[i] == 0) continue;
			point_line.emplace_back(i, line[i]);
		}

		draw(image, point_line);
		return image;
	}


}
