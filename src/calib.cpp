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
#include <mitrax/geometry3d.hpp>


namespace linescan{


	namespace ref3d{


		using namespace mitrax;
		using namespace mitrax::literals;


		constexpr auto plain_x_p0 = make_col_vector< double >(3_R, {  0, 160,  20});
		constexpr auto plain_x_p1 = make_col_vector< double >(3_R, {  0, 160, 160});
		constexpr auto plain_x_p2 = make_col_vector< double >(3_R, {  0,  20,  20});
		constexpr auto plain_x_p3 = make_col_vector< double >(3_R, {  0,  20, 160});
		constexpr auto plain_y_p0 = make_col_vector< double >(3_R, { 20,   0,  20});
		constexpr auto plain_y_p1 = make_col_vector< double >(3_R, { 20,   0, 160});
		constexpr auto plain_y_p2 = make_col_vector< double >(3_R, {160,   0,  20});
		constexpr auto plain_y_p3 = make_col_vector< double >(3_R, {160,   0, 160});

		constexpr std::array< raw_col_vector< double, 2 >, 4 > ref_x{{
			make_col_vector< double >(2_R, {plain_x_p2[1], plain_x_p2[2]}),
			make_col_vector< double >(2_R, {plain_x_p3[1], plain_x_p3[2]}),
			make_col_vector< double >(2_R, {plain_x_p0[1], plain_x_p0[2]}),
			make_col_vector< double >(2_R, {plain_x_p1[1], plain_x_p1[2]})
		}};

		constexpr std::array< raw_col_vector< double, 2 >, 4 > ref_y{{
			make_col_vector< double >(2_R, {plain_y_p0[0], plain_y_p0[2]}),
			make_col_vector< double >(2_R, {plain_y_p1[0], plain_y_p1[2]}),
			make_col_vector< double >(2_R, {plain_y_p2[0], plain_y_p2[2]}),
			make_col_vector< double >(2_R, {plain_y_p3[0], plain_y_p3[2]})
		}};


	}


	template < typename M, size_t C >
	auto reduce(mitrax::col_vector< M, C > const& v){
		using namespace mitrax::literals;

		if(v[v.rows() - 1_R] == 0) throw std::logic_error(
			"can not reduce col_vector dimension, last coordinate is 0"
		);

		return mitrax::make_col_vector_by_function(
			v.rows() - 1_R, [&v](size_t i){
				return v[i] / v[v.rows() - 1_R];
			});
	}

	template < typename M, size_t C >
	auto expand(mitrax::col_vector< M, C > const& v){
		using namespace mitrax::literals;

		return mitrax::make_col_vector_by_function(
			v.rows() + 1_R, [&v](size_t i){
				return i == v.rows() ? 1 : v[i];
			});
	}


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

		auto pred_y = [](auto const& a, auto const& b){
				return a.y() < b.y();
			};

		std::sort(ref_points.begin() + 0, ref_points.begin() + 1, pred_y);
		std::sort(ref_points.begin() + 2, ref_points.begin() + 3, pred_y);
		std::sort(ref_points.begin() + 4, ref_points.begin() + 5, pred_y);
		std::sort(ref_points.begin() + 6, ref_points.begin() + 7, pred_y);

		return ref_points;
	}


	auto plane_projection(
		std::array< point< double >, 4 > const& points,
		std::array< mitrax::raw_col_vector< double, 2 >, 4 > const& target
	){
		using namespace mitrax;
		using namespace mitrax::literals;

		auto x0 = points[0].x();
		auto y0 = points[0].y();
		auto x1 = points[1].x();
		auto y1 = points[1].y();
		auto x2 = points[2].x();
		auto y2 = points[2].y();
		auto x3 = points[3].x();
		auto y3 = points[3].y();

		double tx0 = target[0][0];
		double ty0 = target[0][1];
		double tx1 = target[1][0];
		double ty1 = target[1][1];
		double tx2 = target[2][0];
		double ty2 = target[2][1];
		double tx3 = target[3][0];
		double ty3 = target[3][1];

		auto b = make_matrix< double >(9_C, 9_R, {
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

		auto vec = matrix_kernel(b);

		auto res = make_matrix< double >(3_C, 3_R, {
			{vec[0], vec[1], vec[2]},
			{vec[3], vec[4], vec[5]},
			{vec[6], vec[7], vec[8]}
		});

		std::cout << res << std::endl;

		return res;
	}

	auto plane_calculator(mitrax::raw_matrix< double, 3, 3 > const& m){
		using namespace mitrax;
		using namespace mitrax::literals;

		return [m](point< double > const& p){
			auto p1_3d = m * make_col_vector< double >(
				3_R, {p.x(), p.y(), 1});

			return point< double >(
				p1_3d[0] / p1_3d[2],
				p1_3d[1] / p1_3d[2]
			);
		};
	}


	auto laser_function(){
		using namespace mitrax;
		using namespace mitrax::literals;

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
				= make_matrix< std::uint8_t >(binary.dims());

			draw(lines, calib_line);
			save(lines, "04_calib_line.png");
		}

		return calib_line;
	}

	auto camera_projection(std::array< point< double >, 8 > const& ref_points){
		using namespace mitrax;
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

		constexpr double tx0 = ref3d::plain_x_p0[0]; (void) tx0;
		constexpr double ty0 = ref3d::plain_x_p0[1]; (void) ty0;
		constexpr double tz0 = ref3d::plain_x_p0[2]; (void) tz0;

		constexpr double tx1 = ref3d::plain_x_p1[0]; (void) tx1;
		constexpr double ty1 = ref3d::plain_x_p1[1]; (void) ty1;
		constexpr double tz1 = ref3d::plain_x_p1[2]; (void) tz1;

		constexpr double tx2 = ref3d::plain_x_p2[0]; (void) tx2;
		constexpr double ty2 = ref3d::plain_x_p2[1]; (void) ty2;
		constexpr double tz2 = ref3d::plain_x_p2[2]; (void) tz2;

		constexpr double tx3 = ref3d::plain_x_p3[0]; (void) tx3;
		constexpr double ty3 = ref3d::plain_x_p3[1]; (void) ty3;
		constexpr double tz3 = ref3d::plain_x_p3[2]; (void) tz3;

		constexpr double tx4 = ref3d::plain_y_p0[0]; (void) tx4;
		constexpr double ty4 = ref3d::plain_y_p0[1]; (void) ty4;
		constexpr double tz4 = ref3d::plain_y_p0[2]; (void) tz4;

		constexpr double tx5 = ref3d::plain_y_p1[0]; (void) tx5;
		constexpr double ty5 = ref3d::plain_y_p1[1]; (void) ty5;
		constexpr double tz5 = ref3d::plain_y_p1[2]; (void) tz5;

		constexpr double tx6 = ref3d::plain_y_p2[0]; (void) tx6;
		constexpr double ty6 = ref3d::plain_y_p2[1]; (void) ty6;
		constexpr double tz6 = ref3d::plain_y_p2[2]; (void) tz6;

		constexpr double tx7 = ref3d::plain_y_p3[0]; (void) tx7;
		constexpr double ty7 = ref3d::plain_y_p3[1]; (void) ty7;
		constexpr double tz7 = ref3d::plain_y_p3[2]; (void) tz7;

		auto b = make_matrix< double >(12_C, 12_R, {
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

		auto vec = matrix_kernel(b);

		auto res = make_matrix< double >(4_C, 3_R, {
			{vec[0], vec[1], vec[2], vec[3]},
			{vec[4], vec[5], vec[6], vec[7]},
			{vec[8], vec[9], vec[10], vec[11]}
		});
		std::cout << res << std::endl;

		return res;
	}

	auto camera_calculator(mitrax::raw_matrix< double, 4, 3 > const& m){
		using namespace mitrax;

		return [m](raw_col_vector< double, 3 > const& p){
			auto p_3d = m * expand(p);

			return point< double >(
				p_3d[0] / p_3d[2],
				p_3d[1] / p_3d[2]
			);
		};
	}

	void calib(){
		using namespace mitrax;
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

		auto x_proj = plane_projection(x_plane, ref3d::ref_x);
		auto x_calc = plane_calculator(x_proj);

		auto y_proj = plane_projection(y_plane, ref3d::ref_y);
		auto y_calc = plane_calculator(y_proj);

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

		std::cout << calc(ref3d::plain_x_p0) << std::endl;
		std::cout << calc(ref3d::plain_x_p1) << std::endl;
		std::cout << calc(ref3d::plain_x_p2) << std::endl;
		std::cout << calc(ref3d::plain_x_p3) << std::endl;
		std::cout << calc(ref3d::plain_y_p0) << std::endl;
		std::cout << calc(ref3d::plain_y_p1) << std::endl;
		std::cout << calc(ref3d::plain_y_p2) << std::endl;
		std::cout << calc(ref3d::plain_y_p3) << std::endl;

		auto laser = laser_function();

		auto x = laser.next;

		auto cp0 = point< double >(x, laser(x));
		auto cp1 = point< double >(x - 100, laser(x - 100));
		auto cp2 = point< double >(x + 100, laser(x + 100));

		std::cout << "Point on camera:" << std::endl;
		std::cout << cp0 << std::endl;
		std::cout << cp1 << std::endl;
		std::cout << cp2 << std::endl;

		auto p0_y = y_calc(cp0);
		auto p0_x = x_calc(cp0);
		auto p0 = point< double >(
			(p0_y.x() + p0_x.x()) / 2, (p0_y.y() + p0_x.y()) / 2
		);
		auto p1 = y_calc(cp1);
		auto p2 = x_calc(cp2);

		std::cout << "Points on x or y plane:" << std::endl;
		std::cout << p0_y << std::endl;
		std::cout << p0_x << '\n' << std::endl;
		std::cout << p0 << std::endl;
		std::cout << p1 << std::endl;
		std::cout << p2 << std::endl;

		auto p_0 = make_col_vector< double >(3_R, {0, 0, p0.y()});
		auto p_1 = make_col_vector< double >(3_R, {0, p1.x(), p1.y()});
		auto p_2 = make_col_vector< double >(3_R, {p2.x(), 0, p2.y()});

		std::cout << "Points in 3D:" << std::endl;
		std::cout << p_0 << std::endl;
		std::cout << p_1 << std::endl;
		std::cout << p_2 << std::endl;

		std::cout << "Point on camera projected:" << std::endl;
		std::cout << reduce(proj * expand(p_0)) << std::endl;
		std::cout << reduce(proj * expand(p_1)) << std::endl;
		std::cout << reduce(proj * expand(p_2)) << std::endl;

		auto laser_plane = geometry3d::plane< double >(p_0, p_1, p_2);

// 		auto cv0 = make_col_vector< double >(3_R, {cp0.x(), cp0.y(), 1});
// 		auto cv1 = make_col_vector< double >(3_R, {cp1.x(), cp1.y(), 1});
// 		auto cv2 = make_col_vector< double >(3_R, {cp2.x(), cp2.y(), 1});

// 		auto line0_0 = gaussian_elimination(proj, cv0, make_col_vector< double >(1_R, {1}));
// 		auto line0_1 = gaussian_elimination(proj, cv0 * 2, make_col_vector< double >(1_R, {2}));
// 		auto line1_0 = gaussian_elimination(proj, cv1, make_col_vector< double >(1_R, {1}));
// 		auto line1_1 = gaussian_elimination(proj, cv1 * 2, make_col_vector< double >(1_R, {2}));
// 		auto line2_0 = gaussian_elimination(proj, cv2, make_col_vector< double >(1_R, {1}));
// 		auto line2_1 = gaussian_elimination(proj, cv2 * 2, make_col_vector< double >(1_R, {2}));

// 		std::cout << "Lines in 3D:" << std::endl;
// 		std::cout << line0_0 << " -> " << line0_1 << std::endl;
// 		std::cout << line1_0 << " -> " << line1_1 << std::endl;
// 		std::cout << line2_0 << " -> " << line2_1 << std::endl;
// 
// 		auto line0 = geometry3d::line< double >(reduce(line0_0), reduce(line0_1));
// 		auto line1 = geometry3d::line< double >(reduce(line1_0), reduce(line1_1));
// 		auto line2 = geometry3d::line< double >(reduce(line2_0), reduce(line2_1));
// 
// 		std::cout << "Projected 3D points:" << std::endl;
// 		std::cout << intersect(laser_plane, line0) << std::endl;
// 		std::cout << intersect(laser_plane, line1) << std::endl;
// 		std::cout << intersect(laser_plane, line2) << std::endl;
	}


}
