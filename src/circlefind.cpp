//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/circlefind.hpp>
#include <linescan/processing.hpp>
#include <linescan/median.hpp>
#include <linescan/save.hpp>

#include <mitrax/convolution.hpp>
#include <mitrax/segmentation.hpp>
#include <mitrax/area_search.hpp>

#include <array>
#include <cmath>
#include <iostream>


namespace linescan{


	namespace{


		template < typename T >
		constexpr T sqr(T v){
			return v * v;
		}


		auto edge_scharr_x(mitrax::raw_bitmap< float > const& image){
			using namespace mitrax::literals;

			constexpr auto scharr_x_col =
				mitrax::make_col_vector< float >(3_R, {3, 10, 3});
			constexpr auto scharr_x_row =
				mitrax::make_row_vector< float >(3_C, {1, 0, -1});

			return mitrax::convolution(image, scharr_x_col, scharr_x_row);
		}


		auto edge_scharr_y(mitrax::raw_bitmap< float > const& image){
			using namespace mitrax::literals;

			constexpr auto scharr_y_col =
				mitrax::make_col_vector< float >(3_R, {1, 0, -1});
			constexpr auto scharr_y_row =
				mitrax::make_row_vector< float >(3_C, {3, 10, 3});

			return mitrax::convolution(image, scharr_y_col, scharr_y_row);
		}


		auto edge_scharr_amplitude(mitrax::raw_bitmap< float > const& image){
			return mitrax::transform([](float x, float y){
				return std::sqrt(x * x + y * y);
			}, edge_scharr_x(image), edge_scharr_y(image));
		}


	}


	circle fit_circle(
		mitrax::raw_bitmap< float > const& image,
		float x_from, float x_length, std::size_t x_steps,
		float y_from, float y_length, std::size_t y_steps,
		float r_from, float r_length, std::size_t r_steps
	){
		if (x_from < 0 || y_from < 0 || r_from < 0){
			throw std::logic_error("negative from in fit_circle (min is 2)");
		}

		if (x_length < 0 || y_length < 0 || r_length < 0){
			throw std::logic_error("negative length in fit_circle (min is 2)");
		}

		if (x_steps < 2 || y_steps < 2 || r_steps < 2){
			throw std::logic_error("illegal steps in fit_circle (min is 2)");
		}

		auto point_count = std::size_t((r_from + r_length) * 2 * 3.14159f);

		circle circle;

		auto x_width = x_length / (x_steps - 1);
		auto y_width = y_length / (y_steps - 1);
		auto r_width = r_length / (r_steps - 1);

		auto get = [&image](float x, float y){
			auto ix = static_cast< std::size_t >(x);
			auto iy = static_cast< std::size_t >(y);

			float dx = x - ix;
			float dy = y - iy;

			return
				dx * dy * image(ix, iy) +
				(1 - dx) * dy * image(ix + 1, iy) +
				dx * (1 - dy) * image(ix, iy + 1) +
				(1 - dx) * (1 - dy) * image(ix + 1, iy + 1);
		};

		float max = 0;
		for(std::size_t ri = 0; ri < r_steps; ++ri){
			std::vector< std::array< float, 2 > > pre_calc;
			pre_calc.reserve(point_count);

			auto r = r_from + r_width * ri;

			for(std::size_t i = 0; i < point_count; ++i){
				auto pos = 2 * 3.14159f / point_count * i;
				auto sin = std::sin(pos);
				auto cos = std::cos(pos);

				pre_calc.push_back({{ sin, cos }});
			}

			for(std::size_t myi = 0; myi < y_steps; ++myi){
				auto my = y_from + y_width * myi;

				for(std::size_t mxi = 0; mxi < x_steps; ++mxi){
					auto mx = x_from + x_width * mxi;

					float sum = 0;

					if(
						mx + r >= std::size_t(image.cols()) - 2 ||
						my + r >= std::size_t(image.rows()) - 2 ||
						mx < r - 1 || my < r - 1
					) continue;

					for(std::size_t i = 0; i < point_count; ++i){
						auto x = mx + pre_calc[i][0] * r;
						auto y = my + pre_calc[i][1] * r;

						sum += get(x, y);
					}

					if(sum < max) continue;

					circle.x() = mx;
					circle.y() = my;
					circle.radius() = r;
					max = sum;
				}
			}
		}

		return circle;
	}



	std::tuple< bool, circle, std::uint8_t > find_circle(
		mitrax::raw_bitmap< std::uint8_t > const& image,
		float min_radius, std::uint8_t min_diff
	){
		auto pair = std::minmax_element(image.begin(), image.end());
		auto min = *pair.first;
		auto max = *pair.second;
		auto diff = max - min;
		if(diff < min_diff) return { false, circle(), diff };

		auto threshold = std::uint8_t(min + diff / 2);

		auto binary = binarize(image, threshold);

		auto area = std::size_t(min_radius * min_radius * 3.14159f);

		auto segmentor = mitrax::make_segmentor< false >(binary.dims());

		auto mx = std::size_t(binary.cols()) / 2;
		auto my = std::size_t(binary.rows()) / 2;

		mitrax::point< std::size_t > center(0, 0);
		std::size_t center_count = 0;
		auto success = mitrax::square_area_search(
			binary.dims(), mx, my, mx / 2,
			[&segmentor, &binary, &center, &center_count, area](
				size_t x, size_t y, size_t /*distance*/
			){
				center.set(0, 0);
				center_count = 0;

				auto count = segmentor(x, y,
					[&binary, &center, &center_count](size_t x, size_t y){
						center.x() += x;
						center.y() += y;
						++center_count;
						return !binary(x, y);
					}
				);

				return count > area;
			});

		if(!success) return { false, circle(), diff };

		circle c(
			float(center.x()) / center_count,
			float(center.y()) / center_count,
			std::sqrt(center_count / 3.14159f)
		);

		return { true, c, diff };
	}


	mitrax::raw_bitmap< circle > circlefind(
		mitrax::raw_bitmap< std::uint8_t > const& org,
		std::size_t x_count, std::size_t y_count,
		float radius_in_mm, float distance_in_mm
	){
		using namespace mitrax::literals;

		auto image = median(org, mitrax::dims(3_C, 3_R));

		if(2 * radius_in_mm >= distance_in_mm){
			throw std::logic_error("Circles are overlapping");
		}

		auto const diameter_in_mm = 2 * radius_in_mm;
		auto const space_in_mm = (distance_in_mm - diameter_in_mm);

		auto const max_pixel_per_mm = float(image.cols()) / (
				x_count * diameter_in_mm +
				(x_count - 1) * space_in_mm
			);

		auto const min_pixel_per_mm = max_pixel_per_mm / 2;

		auto const max_radius = radius_in_mm * max_pixel_per_mm;
		auto const min_radius = radius_in_mm * min_pixel_per_mm;

		auto const max_space = space_in_mm * max_pixel_per_mm;


		auto p = mitrax::point< size_t >(
			std::size_t(std::size_t(image.cols()) / 2 - max_radius * 2),
			std::size_t(std::size_t(image.rows()) / 2 - max_radius * 2)
		);

		auto size = mitrax::dims(
			std::size_t(max_radius * 4),
			std::size_t(max_radius * 4)
		);

		auto ref = mitrax::sub_matrix(image, p, size);

		bool success = false;
		circle c;
		std::uint8_t min_diff = 15;
		std::tie(success, c, min_diff) = find_circle(
			ref, min_radius, min_diff
		);

		c.x() += p.x();
		c.y() += p.y();

		auto next_circle_nx = [](mitrax::point< float > point, float length){
			point.x() -= length;
			return point;
		};

		auto next_circle_px = [](mitrax::point< float > point, float length){
			point.x() += length;
			return point;
		};

		auto next_circle_ny = [](mitrax::point< float > point, float length){
			point.y() -= length;
			return point;
		};

		auto next_circle_py = [](mitrax::point< float > point, float length){
			point.y() += length;
			return point;
		};

		auto find_circles = [&image, min_radius, max_space, min_diff](
			circle const& c, auto next_circle
		){
			std::vector< circle > circles{ c };

			auto diff = min_diff;
			for(;;){
				auto space = circles.back().radius() / min_radius * max_space;
				auto pos = circles.back().center();

				pos.x() -= circles.back().radius() + space / 2;
				pos.y() -= circles.back().radius() + space / 2;

				pos = next_circle(
					pos,
					circles.back().diameter() + space / 2
				);

				auto p = mitrax::point< std::size_t >(
					std::size_t(pos.x()),
					std::size_t(pos.y())
				);

				auto size = mitrax::dims(
						circles.back().radius() * 2 + space,
						circles.back().radius() * 2 + space
					);

				if(
					p.x() >= image.cols() ||
					p.y() >= image.rows() ||
					p.x() + size.cols() > image.cols() ||
					p.y() + size.rows() > image.rows()
				){
					return circles;
				}

				circle c;
				auto success = false;
				auto ref = mitrax::sub_matrix(image, p, size);
				std::tie(success, c, diff) = find_circle(
					ref,
					circles.back().radius() * 0.8f,
					std::uint8_t(diff * 0.7f)
				);

				if(!success) return circles;

				c.x() += p.x();
				c.y() += p.y();

				circles.push_back(c);
			}
		};

		auto circles_x_backward = find_circles(c, next_circle_nx);
		auto circles_x_forward = find_circles(c, next_circle_px);

		std::vector< circle > circles_x;
		circles_x.reserve(x_count);

		std::copy(
			circles_x_backward.crbegin(),
			circles_x_backward.crend(),
			std::back_inserter(circles_x)
		);

		std::copy(
			circles_x_forward.cbegin() + 1,
			circles_x_forward.cend(),
			std::back_inserter(circles_x)
		);

		if(circles_x.size() != x_count){
			throw std::runtime_error(
				"circle count in x direction is not correct (" +
				std::to_string(circles_x.size()) + " is not " +
				std::to_string(x_count) + ")"
			);
		}

		auto circles_y_backward =
			find_circles(circles_x.front(), next_circle_ny);

		auto circles_y_forward =
			find_circles(circles_x.front(), next_circle_py);

		std::vector< std::vector< circle > > circles;
		circles.reserve(y_count);

		circles_y_backward.erase(circles_y_backward.begin());
		for(
			auto i = circles_y_backward.rbegin();
			i != circles_y_backward.rend();
			++i
		){
			circles.push_back(std::vector< circle >{*i});
		}

		circles.push_back(std::move(circles_x));

		circles_y_forward.erase(circles_y_forward.begin());
		for(auto const& c: circles_y_forward){
			circles.push_back(std::vector< circle >{c});
		}

		if(circles.size() != y_count){
			throw std::runtime_error(
				"circle count in y direction is not correct (" +
				std::to_string(circles.size()) + " is not " +
				std::to_string(y_count) + ")"
			);
		}

		for(auto& line: circles){
			if(line.size() > 1) continue;

			auto circles_x_backward =
				find_circles(line.front(), next_circle_nx);

			auto circles_x_forward =
				find_circles(line.front(), next_circle_px);

			std::vector< circle > circles_x;
			circles_x.reserve(x_count);

			std::copy(
				circles_x_backward.crbegin(),
				circles_x_backward.crend(),
				std::back_inserter(circles_x)
			);

			std::copy(
				circles_x_forward.cbegin() + 1,
				circles_x_forward.cend(),
				std::back_inserter(circles_x)
			);

			if(circles_x.size() != x_count){
				throw std::runtime_error(
					"circle count in x direction is not correct (" +
					std::to_string(circles_x.size()) + " is not " +
					std::to_string(x_count) + ")"
				);
			}

			line = std::move(circles_x);
		}

		for(auto& line: circles){
			for(auto& c: line){
				c.x() += 1;
				c.y() += 1;
			}
		}

		return mitrax::make_matrix_by_function(
			mitrax::dims(x_count, y_count),
			[&circles](std::size_t x, std::size_t y){
				return circles[y][x];
			}
  		);
	}

	mitrax::raw_bitmap< circle > fine_fit(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		mitrax::raw_bitmap< circle > circles,
		float radius_mm, float distance_mm
	){
		auto space_mm = distance_mm - radius_mm * 2;

		for(auto& c: circles){
			auto r = c.radius();
			auto space = r / radius_mm * space_mm;

			auto pos = mitrax::point< std::size_t >(
				std::size_t(c.x() - r - space / 2 + 0.5f),
				std::size_t(c.y() - r - space / 2 + 0.5f)
			);
			auto dim = mitrax::dims(
				std::size_t(r * 2 + space)
			);
			auto dims = mitrax::dims(dim, dim);

			auto image = mitrax::sub_matrix(bitmap, pos, dims);

			auto scaled = mitrax::make_matrix_by_function(
				(image.dims() + mitrax::dims(2, 2)) / 3,
				[&image](std::size_t x, std::size_t y){
					x *= 3;
					y *= 3;

					std::size_t ex = x + 2;
					std::size_t ey = y + 2;

					if(ex >= image.cols()) x = std::size_t(image.cols()) - 3;
					if(ey >= image.rows()) y = std::size_t(image.rows()) - 3;

					float v = 0;
					for(std::size_t dy = 0; dy < 3; ++dy){
						for(std::size_t dx = 0; dx < 3; ++dx){
							v += image(x + dx, y + dy);
						}
					}

					return v / 9;
				});
			r /= 3;

			auto edge = edge_scharr_amplitude(scaled);

			c = fit_circle(
				edge,
				std::size_t(edge.cols()) / 2.f - r * 0.15f, r * 0.3f, 8,
				std::size_t(edge.rows()) / 2.f - r * 0.15f, r * 0.3f, 8,
				r * 0.85f, r * 0.3f, 20
			);

			c.x() = (c.x() + 1) * 3 + pos.x() + 1;
			c.y() = (c.y() + 1) * 3 + pos.y() + 1;
			c.radius() *= 3;
		}

		return circles;
	}

	circle fine_fit(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		circle const& c, float variance
	){
		auto r = c.radius();
		auto space = r * variance;

		auto pos = mitrax::point< std::size_t >(
			std::size_t(c.x() - r - space / 2 + 0.5f),
			std::size_t(c.y() - r - space / 2 + 0.5f)
		);

		auto dim = mitrax::dims(
			std::size_t(r * 2 + space)
		);

		auto dims = mitrax::dims(dim, dim);

		if(pos.x() + dims.cols() > bitmap.cols()){
			dims.set_cols(bitmap.cols() - mitrax::cols(pos.x()));
		}

		if(pos.y() + dims.rows() > bitmap.rows()){
			dims.set_rows(bitmap.rows() - mitrax::rows(pos.y()));
		}

		auto image = mitrax::sub_matrix(bitmap, pos, dims);

		auto scaled = mitrax::make_matrix_by_function(
			(image.dims() + mitrax::dims(2, 2)) / 3,
			[&image](std::size_t x, std::size_t y){
				x *= 3;
				y *= 3;

				std::size_t ex = x + 2;
				std::size_t ey = y + 2;

				if(ex >= image.cols()) x = std::size_t(image.cols()) - 3;
				if(ey >= image.rows()) y = std::size_t(image.rows()) - 3;

				float v = 0;
				for(std::size_t dy = 0; dy < 3; ++dy){
					for(std::size_t dx = 0; dx < 3; ++dx){
						v += image(x + dx, y + dy);
					}
				}

				return v / 9;
			});
		r /= 3;

		auto edge = edge_scharr_amplitude(scaled);

		auto circle = fit_circle(
			edge,
			std::size_t(edge.cols()) / 2.f - r * 0.15f, r * 0.3f, 16,
			std::size_t(edge.rows()) / 2.f - r * 0.15f, r * 0.3f, 16,
			r * 0.85f, r * 0.3f, 30
		);

		circle.x() = (circle.x() + 1) * 3 + pos.x() + 1;
		circle.y() = (circle.y() + 1) * 3 + pos.y() + 1;
		circle.radius() *= 3;

		return circle;
	}

	circle find_calib_circle(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		mitrax::point< std::size_t > const& pos,
		mitrax::dims_t< 0, 0 > dims
	){
		using namespace mitrax::literals;

		if(pos.x() + dims.cols() > bitmap.cols()){
			dims.set_cols(bitmap.cols() - mitrax::cols(pos.x()));
		}

		if(pos.y() + dims.rows() > bitmap.rows()){
			dims.set_rows(bitmap.rows() - mitrax::rows(pos.y()));
		}

		auto image = mitrax::sub_matrix(bitmap, pos, dims);
		image = median(image, mitrax::dims(3_C, 3_R));

		auto circle_data = find_circle(image, 8, 15);

		if(!std::get< 0 >(circle_data)){
			throw std::runtime_error("circle not found");
		}

		auto circle = std::get< 1 >(circle_data);

		circle.x() += pos.x() + 1;
		circle.y() += pos.y() + 1;

		circle = fine_fit(bitmap, circle, 0.3);

		return circle;
	}

	std::array< circle, 2 > find_calib_line(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap
	){
		using namespace mitrax::literals;

		auto pos1 = mitrax::point< std::size_t >(
			0,
			bitmap.rows() - bitmap.rows() / 5_R
		);

		auto pos2 = mitrax::point< std::size_t >(
			bitmap.cols() - bitmap.cols() / 2_C,
			bitmap.rows() - bitmap.rows() / 5_R
		);

		auto dims = mitrax::dims(
			bitmap.cols() / 2_C,
			bitmap.rows() / 5_R
		);

		return {{
			find_calib_circle(bitmap, pos1, dims),
			find_calib_circle(bitmap, pos2, dims)
		}};
	}

	std::array< circle, 2 > find_calib_line(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		circle const& c1, circle const& c2
	){
		auto d1 = std::size_t(c1.radius() * 1.1f);
		auto pos1 = mitrax::point< std::size_t >(
			c1.x() < d1 ? 0 : c1.x() - d1,
			c1.y() < d1 ? 0 : c1.y() - d1
		);

		auto d2 = std::size_t(c2.radius() * 1.1f);
		auto pos2 = mitrax::point< std::size_t >(
			c2.x() < d2 ? 0 : c2.x() - d2,
			c2.y() < d2 ? 0 : c2.y() - d2
		);

		auto cols = std::size_t(bitmap.cols());
		auto rows = std::size_t(bitmap.rows());

		auto dims1 = mitrax::dims(
			pos1.x() + 2 * d1 > cols ? cols : pos1.x() + 2 * d1,
			pos1.y() + 2 * d1 > rows ? rows : pos1.y() + 2 * d1
		);

		auto dims2 = mitrax::dims(
			pos2.x() + 2 * d2 > cols ? cols : pos2.x() + 2 * d2,
			pos2.y() + 2 * d2 > rows ? rows : pos2.y() + 2 * d2
		);

		return {{
			find_calib_circle(bitmap, pos1, dims1),
			find_calib_circle(bitmap, pos2, dims2)
		}};
	}


}
