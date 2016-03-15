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
#include <linescan/to_image.hpp>
#include <linescan/median.hpp>
#include <linescan/save.hpp>

#include <mitrax/convolution.hpp>
#include <mitrax/segmentation.hpp>
#include <mitrax/area_search.hpp>
#include <mitrax/point_io.hpp>

#include <QtGui/QPainter>

#include <array>
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


	class circle{
	public:
		constexpr circle()noexcept:
			center_(0, 0),
			radius_(0)
			{}

		constexpr circle(float x, float y, float radius)noexcept:
			center_(x, y),
			radius_(radius)
			{}

		constexpr circle(point< float > const& center, float radius)noexcept:
			center_(center),
			radius_(radius)
			{}

		constexpr circle(circle&&)noexcept = default;

		constexpr circle(circle const&)noexcept = default;


		constexpr circle& operator=(circle&&)noexcept = default;

		constexpr circle& operator=(circle const&)noexcept = default;


		constexpr point< float >& center()noexcept{ return center_; }

		constexpr point< float > center()const noexcept{ return center_; }

		constexpr float& x()noexcept{ return center_.x(); }

		constexpr float x()const noexcept{ return center_.x(); }

		constexpr float& y()noexcept{ return center_.y(); }

		constexpr float y()const noexcept{ return center_.y(); }

		constexpr float& radius()noexcept{ return radius_; }

		constexpr float radius()const noexcept{ return radius_; }

		constexpr float diameter()const noexcept{ return radius() * 2; }


	private:
		point< float > center_;
		float radius_;
	};


	circle fit_circle(
		mitrax::raw_bitmap< std::uint8_t > const& image,
		std::uint8_t ref_value,
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

		float min = std::numeric_limits< float >::max();
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

						sum += std::abs(float(get(x, y)) - ref_value);
					}

					if(sum > min) continue;

					circle.x() = mx;
					circle.y() = my;
					circle.radius() = r;
					min = sum;
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

		save(image, "image.png");
		auto binary = binarize(image, threshold);
		save(binary, "binary.png");

		auto area = std::size_t(min_radius * min_radius * 3.14159f);

		auto segmentor = mitrax::make_segmentor< false >(binary.dims());

		auto mx = std::size_t(binary.cols()) / 2;
		auto my = std::size_t(binary.rows()) / 2;

		point< std::size_t > center(0, 0);
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
		save(segmentor.used(), "used.png");

		if(!success) return { false, circle(), diff };

		circle c(
			float(center.x()) / center_count,
			float(center.y()) / center_count,
			std::sqrt(center_count / 3.14159f)
		);

		c = fit_circle(
			image, threshold,
			c.x() - c.radius() * 0.15f, c.radius() * 0.3f, 8,
			c.y() - c.radius() * 0.15f, c.radius() * 0.3f, 8,
			c.radius() * 0.85f, c.radius() * 0.3f, 20
		);

		return { true, c, diff };
	}


	std::vector< double > circlefind(
		mitrax::raw_bitmap< std::uint8_t > const& org,
		std::size_t x_count, std::size_t y_count,
		float radius_in_mm, float distance_in_mm
	){
		using namespace mitrax::literals;

// 		auto image = org;
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
		auto const min_space = space_in_mm * min_pixel_per_mm;


		auto p = point< size_t >(
			std::size_t(std::size_t(image.cols()) / 2 - max_radius * 2),
			std::size_t(std::size_t(image.rows()) / 2 - max_radius * 2)
		);

		auto size = mitrax::dims(
			std::size_t(max_radius * 4),
			std::size_t(max_radius * 4)
		);

		auto ref_middle_image = mitrax::sub_matrix(image, p, size);
// 		save(ref_middle_image, "ref_middle.png");

		bool success = false;
		circle c;
		std::uint8_t min_diff = 15;
		std::tie(success, c, min_diff) = find_circle(
			ref_middle_image, min_radius, min_diff
		);

		c.x() += p.x();
		c.y() += p.y();

		auto next_circle_nx = [](point< float > point, float length){
			point.x() -= length;
			return point;
		};

		auto next_circle_px = [](point< float > point, float length){
			point.x() += length;
			return point;
		};

		auto next_circle_ny = [](point< float > point, float length){
			point.y() -= length;
			return point;
		};

		auto next_circle_py = [](point< float > point, float length){
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
				auto center = circles.back().center();

				center.x() -= circles.back().radius() + space / 2;
				center.y() -= circles.back().radius() + space / 2;

				center = next_circle(
					center,
					circles.back().diameter() + space / 2
				);

				auto p = point< std::size_t >(
					std::size_t(center.x()),
					std::size_t(center.y())
				);

				auto size = mitrax::dims(
						circles.back().radius() * 2 + space,
						circles.back().radius() * 2 + space
					);

				circle c;
				auto success = false;
				auto ref = mitrax::sub_matrix(image, p, size);
				std::tie(success, c, diff) = find_circle(
					ref,
					circles.back().radius() * 0.8f,
					std::uint8_t(diff * 0.5f)
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


		auto debug = to_image(image).convertToFormat(QImage::Format_RGB32);
		{
			QPainter painter(&debug);
			painter.setRenderHint(QPainter::Antialiasing, true);
			painter.setPen(Qt::red);

			for(auto& line: circles){
				for(auto& c: line){
					painter.drawEllipse(
						QPointF(c.x(), c.y()),
						c.radius(), c.radius()
					);
				}
			}
		}
		debug.save("debug.png", "PNG");


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

		std::cout << "ready" << std::endl;

		std::vector< double > result;

		return result;
	}


}
