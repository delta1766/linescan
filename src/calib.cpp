//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/camera.hpp>

#include <linescan/calc_top_distance_line.hpp>
#include <linescan/calc_calibration_lines.hpp>
#include <linescan/collect_points.hpp>
#include <linescan/binarize.hpp>
#include <linescan/erode.hpp>
#include <linescan/invert.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/composed_function.hpp>
#include <linescan/load.hpp>
#include <linescan/save.hpp>

#include <mitrax/output.hpp>
#include <mitrax/point_io.hpp>
#include <mitrax/operator.hpp>
#include <mitrax/gaussian_elimination.hpp>


namespace linescan{


	std::array< point< double >, 8 > load_points(){
		auto image = load("simulation/real2_ref.png");

		auto binary = binarize(image, std::uint8_t(20));

		save(binary, "11_binary.png");

		binary = erode(binary, 3);

		save(binary, "12_erode.png");

		binary = invert(binary);

		save(binary, "13_invert.png");

		auto point_and_counts = collect_points(binary);

		if(point_and_counts.size() < 8){
			throw std::logic_error(
				"not 8 points fount in reference image"
			);
		}else if(point_and_counts.size() > 8){
			// Sort by size
			std::sort(point_and_counts.begin(), point_and_counts.end(),
				[](auto& a, auto& b){
					return a.second < b.second;
				});

			// keep the 8 biggest, remove the rest
			point_and_counts.erase(
				point_and_counts.begin() + 8,
				point_and_counts.end()
			);
		}

		std::array< point< double >, 8 > ref_points{{
			point_and_counts[0].first,
			point_and_counts[1].first,
			point_and_counts[2].first,
			point_and_counts[3].first,
			point_and_counts[4].first,
			point_and_counts[5].first,
			point_and_counts[6].first,
			point_and_counts[7].first,
		}};

		{
			auto image = mitrax::make_matrix< std::uint8_t >(binary.dims());

			draw(image, boost::container::vector< point< double > >(
				ref_points.begin(), ref_points.end())
			);

			save(image, "14_ref.png");
		}

		std::sort(ref_points.begin(), ref_points.end(),
			[](auto const& a, auto const& b){
				return a.x() < b.x();
			});

		std::sort(ref_points.begin() + 0, ref_points.begin() + 1,
			[](auto const& a, auto const& b){
				return a.y() < b.y();
			});

		std::sort(ref_points.begin() + 2, ref_points.begin() + 3,
			[](auto const& a, auto const& b){
				return a.y() < b.y();
			});

		std::sort(ref_points.begin() + 4, ref_points.begin() + 5,
			[](auto const& a, auto const& b){
				return a.y() < b.y();
			});

		std::sort(ref_points.begin() + 6, ref_points.begin() + 8,
			[](auto const& a, auto const& b){
				return a.y() < b.y();
			});

		std::swap(ref_points[4], ref_points[6]);
		std::swap(ref_points[5], ref_points[7]);

		return ref_points;
	}


	auto plane_projection(std::array< point< double >, 4 > const& points){
		using namespace mitrax::literals;

		auto x0 = points[0].x();
		auto y0 = points[0].y();
		auto x1 = points[1].x();
		auto y1 = points[1].y();
		auto x2 = points[2].x();
		auto y2 = points[2].y();
		auto x3 = points[3].x();
		auto y3 = points[3].y();

		double tx0 = 160;
		double ty0 = 160;
		double tx1 = 160;
		double ty1 = 20;
		double tx2 = 20;
		double ty2 = 160;
		double tx3 = 20;
		double ty3 = 20;

		auto b = mitrax::make_matrix< double >(9_C, 9_R, {
			{x0, y0, 1,  0,  0, 0, -tx0 * x0, -tx0 * y0, -tx0},
			{ 0,  0, 0, x0, y0, 1, -ty0 * x0, -ty0 * y0, -ty0},
			{x1, y1, 1,  0,  0, 0, -tx1 * x1, -tx1 * y1, -tx1},
			{ 0,  0, 0, x1, y1, 1, -ty1 * x1, -ty1 * y1, -ty1},
			{x2, y2, 1,  0,  0, 0, -tx2 * x2, -tx2 * y2, -tx2},
			{ 0,  0, 0, x2, y2, 1, -ty2 * x2, -ty2 * y2, -ty2},
			{x3, y3, 1,  0,  0, 0, -tx3 * x3, -tx3 * y3, -tx3},
			{ 0,  0, 0, x3, y3, 1, -ty3 * x3, -ty3 * y3, -ty3},
			{ 0,  0, 0,  0,  0, 0,         0,         0,    0}
		});

		auto vec = mitrax::matrix_kernel(b);

		auto res = mitrax::make_matrix< double >(3_C, 3_R, {
			{vec[0], vec[1], vec[2]},
			{vec[3], vec[4], vec[5]},
			{vec[6], vec[7], vec[8]}
		});

		std::cout << res << std::endl;

		return res;
	}

	auto plane_calculator(mitrax::raw_matrix< double, 3, 3 > const& m){
		using namespace mitrax::literals;

		return [m](mitrax::point< double > const& p){
			auto p1_3d = m * mitrax::make_col_vector< double >(
				3_R, {p.x(), p.y(), 1});

			return mitrax::point< double >(
				p1_3d[0] / p1_3d[2],
				p1_3d[1] / p1_3d[2]
			);
		};
	}


	auto laser_function(){
		auto image =  load("simulation/real2_laser.png");

		auto binary = binarize(image, std::uint8_t(255));

		save(binary, "01_binary.png");

		binary = erode(binary, 3);

		save(binary, "02_erode.png");

		auto line = calc_top_distance_line(binary);

		save(line, binary.rows(), "03_line.png");

		auto lines = calc_calibration_lines(line, 15);
		auto const& line1 = lines.first;
		auto const& line2 = lines.second;

		auto l1 = fit_linear_function< double >(line1.begin(), line1.end());
		auto l2 = fit_linear_function< double >(line2.begin(), line2.end());

		auto calib_line = make_composed_function(l1, intersection(l1, l2), l2);

		{
			auto lines
				= mitrax::make_matrix< std::uint8_t >(binary.dims());

			draw(lines, calib_line);
			save(lines, "04_calib_line.png");
		}

		return calib_line;
	}

	constexpr double tx0 = 160;
	constexpr double ty0 = 0;
	constexpr double tz0 = 160;

	constexpr double tx1 = 160;
	constexpr double ty1 = 0;
	constexpr double tz1 = 20;

	constexpr double tx2 = 20;
	constexpr double ty2 = 0;
	constexpr double tz2 = 160;

	constexpr double tx3 = 20;
	constexpr double ty3 = 0;
	constexpr double tz3 = 20;

	constexpr double tx4 = 0;
	constexpr double ty4 = 160;
	constexpr double tz4 = 160;

	constexpr double tx5 = 0;
	constexpr double ty5 = 160;
	constexpr double tz5 = 20;

	constexpr double tx6 = 0;
	constexpr double ty6 = 20;
	constexpr double tz6 = 160;

	constexpr double tx7 = 0;
	constexpr double ty7 = 20;
	constexpr double tz7 = 20;

	auto camera_projection(std::array< point< double >, 8 > const& ref_points){
		using namespace mitrax::literals;

		auto x0 = ref_points[0].x(); (void) x0;
		auto y0 = ref_points[0].y(); (void) y0;
		auto x1 = ref_points[1].x(); (void) x1;
		auto y1 = ref_points[1].y(); (void) y1;
		auto x2 = ref_points[2].x(); (void) x2;
		auto y2 = ref_points[2].y(); (void) y2;
		auto x3 = ref_points[3].x(); (void) x3;
		auto y3 = ref_points[3].y(); (void) y3;
		auto x4 = ref_points[4].x(); (void) x4;
		auto y4 = ref_points[4].y(); (void) y4;
		auto x5 = ref_points[5].x(); (void) x5;
		auto y5 = ref_points[5].y(); (void) y5;
		auto x6 = ref_points[6].x(); (void) x6;
		auto y6 = ref_points[6].y(); (void) y6;
		auto x7 = ref_points[7].x(); (void) x7;
		auto y7 = ref_points[7].y(); (void) y7;

		auto b = mitrax::make_matrix< double >(12_C, 12_R, {
			{  0,   0,   0, 0, -tx0, -ty0, -tz0, -1,  y0 * tx0,  y0 * ty0,  y0 * tz0,  y0},
			{tx0, ty0, tz0, 1,    0,    0,    0,  0, -x0 * tx0, -x0 * ty0, -x0 * tz0, -x0},
// 			{  0,   0,   0, 0, -tx1, -ty1, -tz1, -1,  y1 * tx1,  y1 * ty1,  y1 * tz1,  y1},
// 			{tx1, ty1, tz1, 1,    0,    0,    0,  0, -x1 * tx1, -x1 * ty1, -x1 * tz1, -x1},
			{  0,   0,   0, 0, -tx2, -ty2, -tz2, -1,  y2 * tx2,  y2 * ty2,  y2 * tz2,  y2},
			{tx2, ty2, tz2, 1,    0,    0,    0,  0, -x2 * tx2, -x2 * ty2, -x2 * tz2, -x2},
			{  0,   0,   0, 0, -tx3, -ty3, -tz3, -1,  y3 * tx3,  y3 * ty3,  y3 * tz3,  y3},
// 			{tx3, ty3, tz3, 1,    0,    0,    0,  0, -x3 * tx3, -x3 * ty3, -x3 * tz3, -x3},
			{  0,   0,   0, 0, -tx4, -ty4, -tz4, -1,  y4 * tx4,  y4 * ty4,  y4 * tz4,  y4},
			{tx4, ty4, tz4, 1,    0,    0,    0,  0, -x4 * tx4, -x4 * ty4, -x4 * tz4, -x4},
			{  0,   0,   0, 0, -tx5, -ty5, -tz5, -1,  y5 * tx5,  y5 * ty5,  y5 * tz5,  y5},
			{tx5, ty5, tz5, 1,    0,    0,    0,  0, -x5 * tx5, -x5 * ty5, -x5 * tz5, -x5},
// 			{  0,   0,   0, 0, -tx6, -ty6, -tz6, -1,  y6 * tx6,  y6 * ty6,  y6 * tz6,  y6},
// 			{tx6, ty6, tz6, 1,    0,    0,    0,  0, -x6 * tx6, -x6 * ty6, -x6 * tz6, -x6},
			{  0,   0,   0, 0, -tx7, -ty7, -tz7, -1,  y7 * tx7,  y7 * ty7,  y7 * tz7,  y7},
			{tx7, ty7, tz7, 1,    0,    0,    0,  0, -x7 * tx7, -x7 * ty7, -x7 * tz7, -x7},
			{  0,   0,   0, 0,    0,    0,    0,  0,         0,         0,         0,   0}
		});

		auto vec = mitrax::matrix_kernel(b);

		auto res = mitrax::make_matrix< double >(4_C, 3_R, {
			{vec[0], vec[1], vec[2], vec[3]},
			{vec[4], vec[5], vec[6], vec[7]},
			{vec[8], vec[9], vec[10], vec[11]}
		});
		std::cout << res << std::endl;

		return res;
	}

	auto camera_calculator(mitrax::raw_matrix< double, 4, 3 > const& m){
		using namespace mitrax::literals;

		return [m](mitrax::raw_col_vector< double, 4 > const& p){
			auto p_3d = m * p;

			return mitrax::point< double >(
				p_3d[0] / p_3d[2],
				p_3d[1] / p_3d[2]
			);
		};
	}


	void calib(){
		using namespace mitrax::literals;

		auto ref_points = load_points();

		std::array< point< double >, 4 > y_plane{{
			ref_points[0],
			ref_points[1],
			ref_points[2],
			ref_points[3]
		}};

		std::array< point< double >, 4 > x_plane{{
			ref_points[4],
			ref_points[5],
			ref_points[6],
			ref_points[7]
		}};

		auto y_proj = plane_projection(y_plane);
		auto y_calc = plane_calculator(y_proj);

		auto x_proj = plane_projection(x_plane);
		auto x_calc = plane_calculator(x_proj);

		for(auto& v: ref_points) std::cout << v << std::endl;

		std::cout << y_calc(ref_points[0]) << std::endl;
		std::cout << y_calc(ref_points[1]) << std::endl;
		std::cout << y_calc(ref_points[2]) << std::endl;
		std::cout << y_calc(ref_points[3]) << std::endl;
		std::cout << x_calc(ref_points[4]) << std::endl;
		std::cout << x_calc(ref_points[5]) << std::endl;
		std::cout << x_calc(ref_points[6]) << std::endl;
		std::cout << x_calc(ref_points[7]) << std::endl;

		auto proj = camera_projection(ref_points);
		auto calc = camera_calculator(proj);

		std::cout << calc(mitrax::make_col_vector< double >(4_R, {tx0, ty0, tz0})) << std::endl;
		std::cout << calc(mitrax::make_col_vector< double >(4_R, {tx1, ty1, tz1})) << std::endl;
		std::cout << calc(mitrax::make_col_vector< double >(4_R, {tx2, ty2, tz2})) << std::endl;
		std::cout << calc(mitrax::make_col_vector< double >(4_R, {tx3, ty3, tz3})) << std::endl;
		std::cout << calc(mitrax::make_col_vector< double >(4_R, {tx4, ty4, tz4})) << std::endl;
		std::cout << calc(mitrax::make_col_vector< double >(4_R, {tx5, ty5, tz5})) << std::endl;
		std::cout << calc(mitrax::make_col_vector< double >(4_R, {tx6, ty6, tz6})) << std::endl;
		std::cout << calc(mitrax::make_col_vector< double >(4_R, {tx7, ty7, tz7})) << std::endl;

		auto laser = laser_function();
	}


}
