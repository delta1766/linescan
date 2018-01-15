//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/circle.hpp>
#include <linescan/processing.hpp>
#include <linescan/median.hpp>

#include <mitrax/convolution.hpp>
#include <mitrax/segmentation.hpp>
#include <mitrax/area_search.hpp>

#include <cmath>


namespace linescan{


	namespace{


		template < typename T >
		constexpr T sqr(T v){
			return v * v;
		}


		auto edge_scharr_x(mitrax::std_bitmap< float > const& image){
			using namespace mitrax::literals;

			constexpr auto scharr_x_col =
				mitrax::make_vector< float >(3_R, {3, 10, 3});
			constexpr auto scharr_x_row =
				mitrax::make_vector< float >(3_C, {1, 0, -1});

			return mitrax::convolution(image, scharr_x_col, scharr_x_row);
		}


		auto edge_scharr_y(mitrax::std_bitmap< float > const& image){
			using namespace mitrax::literals;

			constexpr auto scharr_y_col =
				mitrax::make_vector< float >(3_R, {1, 0, -1});
			constexpr auto scharr_y_row =
				mitrax::make_vector< float >(3_C, {3, 10, 3});

			return mitrax::convolution(image, scharr_y_col, scharr_y_row);
		}


		auto edge_scharr_amplitude(mitrax::std_bitmap< float > const& image){
			return mitrax::transform([](float x, float y){
				return std::sqrt(x * x + y * y);
			}, edge_scharr_x(image), edge_scharr_y(image));
		}


	}


	circle fit_circle(
		mitrax::std_bitmap< float > const& image,
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
		mitrax::std_bitmap< std::uint8_t > const& org_image,
		float min_radius, std::uint8_t min_diff
	){
		constexpr auto K = 5;
		auto image = median(median(org_image, mitrax::auto_dim_pair_t< K, K >()),
			mitrax::auto_dim_pair_t< K, K >());

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
			// (K / 2) * 2 -> integer division!
			float(center.x()) / center_count + (K / 2) * 2,
			float(center.y()) / center_count + (K / 2) * 2,
			std::sqrt(center_count / 3.14159f)
		);

		return { true, c, diff };
	}

	circle fine_fit(
		mitrax::std_bitmap< std::uint8_t > const& bitmap,
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

		auto dims = mitrax::dim_pair(dim, dim);

		if(pos.x() + dims.cols() > bitmap.cols()){
			dims.set_cols(bitmap.cols() - mitrax::cols(pos.x()));
		}

		if(pos.y() + dims.rows() > bitmap.rows()){
			dims.set_rows(bitmap.rows() - mitrax::rows(pos.y()));
		}

		auto image = mitrax::sub_matrix(bitmap, pos, dims);

		using namespace mitrax::literals;
		auto scaled = mitrax::make_matrix_fn(
			(image.dims() + mitrax::dim_pair(2, 2)) / 3_D,
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

	std::pair< circle, bool > find_calib_circle(
		mitrax::std_bitmap< std::uint8_t > const& bitmap,
		mitrax::point< std::size_t > const& pos,
		mitrax::rt_dim_pair_t dims
	){
		using namespace mitrax::literals;

		auto image = mitrax::sub_matrix(bitmap, pos, dims);
		image = median(median(image, mitrax::dim_pair(5_C, 5_R)),
			mitrax::dim_pair(5_C, 5_R));

		auto circle_data = find_circle(image, 8, 15);

		if(!std::get< 0 >(circle_data)) return { circle(), false };

		auto circle = std::get< 1 >(circle_data);

		circle.x() += pos.x() + 4;
		circle.y() += pos.y() + 4;

		circle = fine_fit(bitmap, circle, 1);

		return { circle, true };
	}

	std::vector< circle > find_calib_circles(
		mitrax::std_bitmap< std::uint8_t > const& bitmap
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

		auto dims = mitrax::dim_pair(
			bitmap.cols() / 2_C,
			bitmap.rows() / 5_R
		);

		auto circle1 = find_calib_circle(bitmap, pos1, dims);
		auto circle2 = find_calib_circle(bitmap, pos2, dims);

		std::vector< circle > result;
		result.reserve(2);
		if(circle1.second) result.push_back(circle1.first);
		if(circle2.second) result.push_back(circle2.first);

		return result;
	}

	std::vector< circle > find_calib_circles(
		mitrax::std_bitmap< std::uint8_t > const& bitmap,
		circle const& c1, circle const& c2
	){
		auto d1 = std::size_t(c1.radius() * 2);
		auto pos1 = mitrax::point< std::size_t >(
			c1.x() < d1 ? 0 : c1.x() - d1,
			c1.y() < d1 ? 0 : c1.y() - d1
		);

		auto d2 = std::size_t(c2.radius() * 2);
		auto pos2 = mitrax::point< std::size_t >(
			c2.x() < d2 ? 0 : c2.x() - d2,
			c2.y() < d2 ? 0 : c2.y() - d2
		);

		auto cols = std::size_t(bitmap.cols());
		auto rows = std::size_t(bitmap.rows());

		auto dims1 = mitrax::dim_pair(
			pos1.x() + 2 * d1 > cols ? cols - pos1.x() : 2 * d1,
			pos1.y() + 2 * d1 > rows ? rows - pos1.y() : 2 * d1
		);

		auto dims2 = mitrax::dim_pair(
			pos2.x() + 2 * d2 > cols ? cols - pos2.x() : 2 * d2,
			pos2.y() + 2 * d2 > rows ? rows - pos2.y() : 2 * d2
		);

		auto circle1 = find_calib_circle(bitmap, pos1, dims1);
		auto circle2 = find_calib_circle(bitmap, pos2, dims2);

		std::vector< circle > result;
		result.reserve(2);
		if(circle1.second) result.push_back(circle1.first);
		if(circle2.second) result.push_back(circle2.first);

		return result;
	}


}
