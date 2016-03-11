//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/circlefind.hpp>
#include <linescan/binarize.hpp>
#include <linescan/save.hpp>

#include <mitrax/convolution.hpp>
#include <mitrax/segmentation.hpp>
#include <mitrax/area_search.hpp>

#include <iostream>


namespace linescan{


	namespace{


		template < typename T >
		constexpr T sqr(T v){
			return v * v;
		}


		auto make_annulus_pattern(float radius, float length){
			auto size = std::size_t((radius + length) * 2) + 1;
			auto mx = size / 2.f - .5f;
			auto my = size / 2.f - .5f;

			return mitrax::make_square_matrix_by_function(mitrax::dims(size),
				[mx, my, radius, length](std::size_t x, std::size_t y){
					auto circle = std::sqrt(sqr(mx - x) + sqr(my - y));

					if(circle < radius || circle > radius + length){
						return false;
					}

					return true;
				});
		}


		auto make_circle_pattern(float radius){
			auto size = std::size_t(radius * 2) + 1;
			auto mx = size / 2.f - .5f;
			auto my = size / 2.f - .5f;

			return mitrax::make_square_matrix_by_function(mitrax::dims(size),
				[mx, my, radius](std::size_t x, std::size_t y){
					auto circle = std::sqrt(sqr(mx - x) + sqr(my - y));

					if(circle > radius) return true;
					return false;
				});
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


	}


	std::vector< double > circlefind(
		mitrax::raw_bitmap< std::uint8_t > const& image,
		std::size_t x_count, std::size_t y_count,
		float radius_in_mm, float distance_in_mm
	){
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

		auto const max_space_radius = space_in_mm * max_pixel_per_mm;
		auto const min_space_radius = space_in_mm * min_pixel_per_mm;


		auto ref_middle_image = mitrax::sub_matrix(
			image,
			std::size_t(std::size_t(image.cols()) / 2 - max_radius * 2),
			std::size_t(std::size_t(image.rows()) / 2 - max_radius * 2),
			mitrax::cols(std::size_t(max_radius * 4)),
			mitrax::rows(std::size_t(max_radius * 4))
		);
		save(ref_middle_image, "ref_middle.png");


		auto pair = std::minmax_element(
			ref_middle_image.begin(), ref_middle_image.end()
		);
		auto min = *pair.first;
		auto max = *pair.second;

		auto binary = binarize(
			ref_middle_image, std::uint8_t(min + (max - min) / 2)
		);
		save(binary, "binary.png");

		auto area = std::size_t(min_radius * min_radius * 3.14159f);

		auto segmentor = mitrax::make_segmentor< false >(binary.dims());

		auto mx = std::size_t(binary.cols()) / 2;
		auto my = std::size_t(binary.rows()) / 2;

		auto success = mitrax::square_area_search(
			binary.dims(), mx, my, mx / 2,
			[&segmentor, &binary, area](
				size_t x, size_t y, size_t distance
			){
				auto count = segmentor(x, y,
					[&binary, area](size_t x, size_t y){
						return !binary(x, y);
					}
				);

				return count > area;
			});

		save(segmentor.used(), "used.png");

// 		auto edge_x = edge_scharr_x(ref_middle_image);
// 		auto edge_y = edge_scharr_y(ref_middle_image);
// 		save(edge_x, "edge_x.png");
// 		save(edge_y, "edge_y.png");
// 
// 		auto aptitude = mitrax::transform([](float x, float y){
// 			return std::sqrt(sqr(x) + sqr(y));
// 		}, edge_x, edge_y);
// 		save(aptitude, "aptitude.png");
// 
// 		auto direction = mitrax::transform([](float x, float y){
// 			return std::atan2(x, y);
// 		}, edge_x, edge_y);
// 		save(direction, "direction.png");
// 
// 
// 		auto pattern = make_annulus_pattern(
// 			min_radius, max_radius - min_radius
// 		);
// 		save(pattern, "pattern.png");
// 
// 
// 		auto max = mitrax::convolution(aptitude, pattern,
// 			[](float a, bool b){ return b ? sqr(a) : -sqr(a); });
// 		save(max, "max.png");

		std::vector< double > result;

		return result;
	}


}
