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
#include <mitrax/householder_transformation.hpp>
#include <mitrax/gauss_newton_algorithm.hpp>

#include <iomanip>
#include <cmath>


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
			make_col_vector< double >(2_R, {plain_y_p2[0], plain_y_p2[2]}),
			make_col_vector< double >(2_R, {plain_y_p3[0], plain_y_p3[2]}),
			make_col_vector< double >(2_R, {plain_y_p0[0], plain_y_p0[2]}),
			make_col_vector< double >(2_R, {plain_y_p1[0], plain_y_p1[2]})
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
// 		auto image = load("simulation/real2_ref.png");
		auto image = load("simulation/measure2/calib0000.png");

		auto binary = binarize(image, std::uint8_t(50));

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
					return a.second > b.second;
				});

			for(auto& v: point_and_counts){
				std::cout << std::setw(8) << v.second << ": " << v.first
					<< std::endl;
			}

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

			draw_cycle(image, ref_points[0], 10.);
			draw_cycle(image, ref_points[1], 10.);
			draw_cycle(image, ref_points[2], 10.);
			draw_cycle(image, ref_points[3], 10.);
			draw_cycle(image, ref_points[4], 10.);
			draw_cycle(image, ref_points[5], 10.);
			draw_cycle(image, ref_points[6], 10.);
			draw_cycle(image, ref_points[7], 10.);

			save(image, "14_ref.png");
		}

		std::sort(ref_points.begin(), ref_points.end(),
			[](auto const& a, auto const& b){
				return a.x() < b.x();
			});

		auto pred_y = [](auto const& a, auto const& b){
				return a.y() < b.y();
			};

		std::sort(ref_points.begin() + 0, ref_points.begin() + 2, pred_y);
		std::sort(ref_points.begin() + 2, ref_points.begin() + 4, pred_y);
		std::sort(ref_points.begin() + 4, ref_points.begin() + 6, pred_y);
		std::sort(ref_points.begin() + 6, ref_points.begin() + 8, pred_y);

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

		std::cout << "Point map 2D" << std::endl;
		std::cout
			<< std::setw(9) << x0 << "x"
			<< std::setw(9) << y0 << " <- "
			<< std::setw(3) << tx0 << "x"
			<< std::setw(3) << ty0 << std::endl;
		std::cout
			<< std::setw(9) << x1 << "x"
			<< std::setw(9) << y1 << " <- "
			<< std::setw(3) << tx1 << "x"
			<< std::setw(3) << ty1 << std::endl;
		std::cout
			<< std::setw(9) << x2 << "x"
			<< std::setw(9) << y2 << " <- "
			<< std::setw(3) << tx2 << "x"
			<< std::setw(3) << ty2 << std::endl;
		std::cout
			<< std::setw(9) << x3 << "x"
			<< std::setw(9) << y3 << " <- "
			<< std::setw(3) << tx3 << "x"
			<< std::setw(3) << ty3 << std::endl;

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

		std::cout << "2D-Transform: " << res << std::endl;

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

// 		auto image = load("simulation/real2_laser.png");
		auto image =  load("simulation/measure2/laser0000.png");

		auto binary = binarize(image, std::uint8_t(255));

		save(binary, "01_binary.png");

		binary = erode(binary, 3);

		save(binary, "02_erode.png");

		auto line = calc_top_distance_line(binary);

		save(
			draw_top_distance_line(line, line.size(), binary.rows()),
			"03_line.png"
		);

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

		std::cout << "Point map 3D" << std::endl;
		std::cout
			<< std::setw(9) << x0 << "x"
			<< std::setw(9) << y0 << " <- "
			<< std::setw(3) << tx0 << "x"
			<< std::setw(3) << ty0 << "x"
			<< std::setw(3) << tz0 << std::endl;
		std::cout
			<< std::setw(9) << x1 << "x"
			<< std::setw(9) << y1 << " <- "
			<< std::setw(3) << tx1 << "x"
			<< std::setw(3) << ty1 << "x"
			<< std::setw(3) << tz1 << std::endl;
		std::cout
			<< std::setw(9) << x2 << "x"
			<< std::setw(9) << y2 << " <- "
			<< std::setw(3) << tx2 << "x"
			<< std::setw(3) << ty2 << "x"
			<< std::setw(3) << tz2 << std::endl;
		std::cout
			<< std::setw(9) << x3 << "x"
			<< std::setw(9) << y3 << " <- "
			<< std::setw(3) << tx3 << "x"
			<< std::setw(3) << ty3 << "x"
			<< std::setw(3) << tz3 << std::endl;
		std::cout
			<< std::setw(9) << x4 << "x"
			<< std::setw(9) << y4 << " <- "
			<< std::setw(3) << tx4 << "x"
			<< std::setw(3) << ty4 << "x"
			<< std::setw(3) << tz4 << std::endl;
		std::cout
			<< std::setw(9) << x5 << "x"
			<< std::setw(9) << y5 << " <- "
			<< std::setw(3) << tx5 << "x"
			<< std::setw(3) << ty5 << "x"
			<< std::setw(3) << tz5 << std::endl;
		std::cout
			<< std::setw(9) << x6 << "x"
			<< std::setw(9) << y6 << " <- "
			<< std::setw(3) << tx6 << "x"
			<< std::setw(3) << ty6 << "x"
			<< std::setw(3) << tz6 << std::endl;
		std::cout
			<< std::setw(9) << x7 << "x"
			<< std::setw(9) << y7 << " <- "
			<< std::setw(3) << tx7 << "x"
			<< std::setw(3) << ty7 << "x"
			<< std::setw(3) << tz7 << std::endl;

// 		auto b = make_matrix< double >(12_C, 12_R, {
// 			{  0,   0,   0, 0, -tx0, -ty0, -tz0, -1,  y0 * tx0,  y0 * ty0,  y0 * tz0,  y0},
// 			{tx0, ty0, tz0, 1,    0,    0,    0,  0, -x0 * tx0, -x0 * ty0, -x0 * tz0, -x0},
// // 			{  0,   0,   0, 0, -tx1, -ty1, -tz1, -1,  y1 * tx1,  y1 * ty1,  y1 * tz1,  y1},
// // 			{tx1, ty1, tz1, 1,    0,    0,    0,  0, -x1 * tx1, -x1 * ty1, -x1 * tz1, -x1},
// 			{  0,   0,   0, 0, -tx2, -ty2, -tz2, -1,  y2 * tx2,  y2 * ty2,  y2 * tz2,  y2},
// 			{tx2, ty2, tz2, 1,    0,    0,    0,  0, -x2 * tx2, -x2 * ty2, -x2 * tz2, -x2},
// 			{  0,   0,   0, 0, -tx3, -ty3, -tz3, -1,  y3 * tx3,  y3 * ty3,  y3 * tz3,  y3},
// // 			{tx3, ty3, tz3, 1,    0,    0,    0,  0, -x3 * tx3, -x3 * ty3, -x3 * tz3, -x3},
// 			{  0,   0,   0, 0, -tx4, -ty4, -tz4, -1,  y4 * tx4,  y4 * ty4,  y4 * tz4,  y4},
// 			{tx4, ty4, tz4, 1,    0,    0,    0,  0, -x4 * tx4, -x4 * ty4, -x4 * tz4, -x4},
// 			{  0,   0,   0, 0, -tx5, -ty5, -tz5, -1,  y5 * tx5,  y5 * ty5,  y5 * tz5,  y5},
// 			{tx5, ty5, tz5, 1,    0,    0,    0,  0, -x5 * tx5, -x5 * ty5, -x5 * tz5, -x5},
// // 			{  0,   0,   0, 0, -tx6, -ty6, -tz6, -1,  y6 * tx6,  y6 * ty6,  y6 * tz6,  y6},
// // 			{tx6, ty6, tz6, 1,    0,    0,    0,  0, -x6 * tx6, -x6 * ty6, -x6 * tz6, -x6},
// 			{  0,   0,   0, 0, -tx7, -ty7, -tz7, -1,  y7 * tx7,  y7 * ty7,  y7 * tz7,  y7},
// 			{tx7, ty7, tz7, 1,    0,    0,    0,  0, -x7 * tx7, -x7 * ty7, -x7 * tz7, -x7},
// 			{  0,   0,   0, 0,    0,    0,    0,  0,         0,         0,         0,   0}
// 		});
// 
// 		auto vec = matrix_kernel(b);
// 		auto res = make_matrix< double >(4_C, 3_R, {
// 			{vec[0], vec[1], vec[2], vec[3]},
// 			{vec[4], vec[5], vec[6], vec[7]},
// 			{vec[8], vec[9], vec[10], vec[11]}
// 		});

		auto f = [](
				raw_col_vector< double, 11 > const& p,
				std::tuple<
					raw_col_vector< double, 3 >,
					raw_col_vector< double, 4 >
				> const& point
			){
				using std::sin;
				using std::cos;

				auto image_point = std::get< 0 >(point);
				auto world_point = std::get< 1 >(point);
				auto K = make_matrix< double >(3_C, 3_R, {
					{p[0], 0, p[1]},
					{0, p[0], p[2]},
					{0, 0, 1}
				});
				auto Rx = make_matrix< double >(3_C, 3_R, {
					{1, 0, 0},
					{0, cos(p[3]), -sin(p[3])},
					{0, sin(p[3]), cos(p[3])},
				});
				auto Ry = make_matrix< double >(3_C, 3_R, {
					{cos(p[4]), 0, sin(p[4])},
					{0, 1, 0},
					{-sin(p[4]), 0, cos(p[4])},
				});
				auto Rz = make_matrix< double >(3_C, 3_R, {
					{cos(p[5]), -sin(p[5]), 0},
					{sin(p[5]), cos(p[5]), 0},
					{0, 0, 1},
				});
				auto C = make_matrix< double >(4_C, 3_R, {
					{1, 0, 0, p[8]},
					{0, 1, 0, p[9]},
					{0, 0, 1, p[10]}
				});
				auto proj = K * Rx * Ry * Rz * C;
				return vector_norm_2sqr(proj * world_point - image_point);
			};

		auto vec3 = [](auto const& p){
			return make_col_vector< double >(3_R, {p.x(), p.y(), 1});
		};

		auto vec4 = [](auto const& p){
			return expand(p);
		};

		boost::container::vector< std::tuple<
			raw_col_vector< double, 3 >,
			raw_col_vector< double, 4 >
		> > data{
			std::make_tuple(vec3(ref_points[0]), vec4(ref3d::plain_x_p0)),
			std::make_tuple(vec3(ref_points[1]), vec4(ref3d::plain_x_p1)),
			std::make_tuple(vec3(ref_points[2]), vec4(ref3d::plain_x_p2)),
			std::make_tuple(vec3(ref_points[3]), vec4(ref3d::plain_x_p3)),
			std::make_tuple(vec3(ref_points[4]), vec4(ref3d::plain_y_p0)),
			std::make_tuple(vec3(ref_points[5]), vec4(ref3d::plain_y_p1)),
			std::make_tuple(vec3(ref_points[6]), vec4(ref3d::plain_y_p2)),
			std::make_tuple(vec3(ref_points[7]), vec4(ref3d::plain_y_p3))
		};

		auto start = make_col_vector< double >(11_R, {
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
		});

		auto p = levenberg_marquardt_algorithm(
			f, start, 1e-10, 0.2, 0.8, 1., data
		);

		auto K = make_matrix< double >(3_C, 3_R, {
			{p[0], 0, p[1]},
			{0, p[0], p[2]},
			{0, 0, 1}
		});
		auto Rx = make_matrix< double >(3_C, 3_R, {
			{1, 0, 0},
			{0, cos(p[3]), -sin(p[3])},
			{0, sin(p[3]), cos(p[3])},
		});
		auto Ry = make_matrix< double >(3_C, 3_R, {
			{cos(p[4]), 0, sin(p[4])},
			{0, 1, 0},
			{-sin(p[4]), 0, cos(p[4])},
		});
		auto Rz = make_matrix< double >(3_C, 3_R, {
			{cos(p[5]), -sin(p[5]), 0},
			{sin(p[5]), cos(p[5]), 0},
			{0, 0, 1},
		});
		auto C = make_matrix< double >(4_C, 3_R, {
			{1, 0, 0, p[8]},
			{0, 1, 0, p[9]},
			{0, 0, 1, p[10]}
		});
		auto res = K * Rx * Ry * Rz * C;
		std::cout << "Parameter: " << p << std::endl;

// 		auto X = make_matrix< double >(8_C, 16_R, {
// 			{ tx0, ty0, tz0, 1,   0,   0,   0, 0 },
// 			{   0,   0,   0, 0, tx0, ty0, tz0, 1 },
// 			{ tx1, ty1, tz1, 1,   0,   0,   0, 0 },
// 			{   0,   0,   0, 0, tx1, ty1, tz1, 1 },
// 			{ tx2, ty2, tz2, 1,   0,   0,   0, 0 },
// 			{   0,   0,   0, 0, tx2, ty2, tz2, 1 },
// 			{ tx3, ty3, tz3, 1,   0,   0,   0, 0 },
// 			{   0,   0,   0, 0, tx3, ty3, tz3, 1 },
// 			{ tx4, ty4, tz4, 1,   0,   0,   0, 0 },
// 			{   0,   0,   0, 0, tx4, ty4, tz4, 1 },
// 			{ tx5, ty5, tz5, 1,   0,   0,   0, 0 },
// 			{   0,   0,   0, 0, tx5, ty5, tz5, 1 },
// 			{ tx6, ty6, tz6, 1,   0,   0,   0, 0 },
// 			{   0,   0,   0, 0, tx6, ty6, tz6, 1 },
// 			{ tx7, ty7, tz7, 1,   0,   0,   0, 0 },
// 			{   0,   0,   0, 0, tx7, ty7, tz7, 1 }
// 		});
// 
// 		auto tX = transpose(X);
// 		auto piX = inverse(tX * X) * tX;
// 
// 		auto b = make_col_vector< double >(16_R, {
// 			ref_points[0].x(),
// 			ref_points[0].y(),
// 			ref_points[1].x(),
// 			ref_points[1].y(),
// 			ref_points[2].x(),
// 			ref_points[2].y(),
// 			ref_points[3].x(),
// 			ref_points[3].y(),
// 			ref_points[4].x(),
// 			ref_points[4].y(),
// 			ref_points[5].x(),
// 			ref_points[5].y(),
// 			ref_points[6].x(),
// 			ref_points[6].y(),
// 			ref_points[7].x(),
// 			ref_points[7].y()
// 		});
// 
// 		auto vec = piX * b;
// 		auto res = make_matrix< double >(4_C, 3_R, {
// 			{vec[0], vec[1], vec[2], vec[3]},
// 			{vec[4], vec[5], vec[6], vec[7]},
// 			{     0,      0,      0,      1}
// 		});

		std::cout << "3D-Transform" << res << std::endl;

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

		std::cout << std::defaultfloat;

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

		std::cout << "Check 2D-transforms" << std::endl;
		std::cout << "160x 20 == " << y_calc(ref_points[0]) << std::endl;
		std::cout << "160x160 == " << y_calc(ref_points[1]) << std::endl;
		std::cout << " 20x 20 == " << y_calc(ref_points[2]) << std::endl;
		std::cout << " 20x160 == " << y_calc(ref_points[3]) << std::endl;
		std::cout << " 20x 20 == " << x_calc(ref_points[4]) << std::endl;
		std::cout << " 20x160 == " << x_calc(ref_points[5]) << std::endl;
		std::cout << "160x 20 == " << x_calc(ref_points[6]) << std::endl;
		std::cout << "160x160 == " << x_calc(ref_points[7]) << std::endl;

		auto proj = camera_projection(ref_points);
// 		auto proj = make_matrix< double >(4_C, 3_R, {
// 			{2.07993277617415, -2.01928991667109, -0.124721428222842, 528.204749843440},
// 			{0.836162087933566, 0.733748626395102, 2.85204821409199, 33.1228035857583},
// 			{0, 0, 0, 1}
// 		});
		auto calc = camera_calculator(proj);

		std::cout << "Check 3D-transform" << std::endl;
		std::cout << ref_points[0] << " == " << calc(ref3d::plain_x_p0) << std::endl;
		std::cout << ref_points[1] << " == " << calc(ref3d::plain_x_p1) << std::endl;
		std::cout << ref_points[2] << " == " << calc(ref3d::plain_x_p2) << std::endl;
		std::cout << ref_points[3] << " == " << calc(ref3d::plain_x_p3) << std::endl;
		std::cout << ref_points[4] << " == " << calc(ref3d::plain_y_p0) << std::endl;
		std::cout << ref_points[5] << " == " << calc(ref3d::plain_y_p1) << std::endl;
		std::cout << ref_points[6] << " == " << calc(ref3d::plain_y_p2) << std::endl;
		std::cout << ref_points[7] << " == " << calc(ref3d::plain_y_p3) << std::endl;

// 		auto laser = laser_function();
// 
// 		auto x = laser.next;
// 
// 		auto cp0 = point< double >(x, laser(x));
// 		auto cp1 = point< double >(x - 100, laser(x - 100));
// 		auto cp2 = point< double >(x + 100, laser(x + 100));
// 
// 		std::cout << "Point on camera:" << std::endl;
// 		std::cout << cp0 << std::endl;
// 		std::cout << cp1 << std::endl;
// 		std::cout << cp2 << std::endl;
// 
// 		auto p0_y = y_calc(cp0);
// 		auto p0_x = x_calc(cp0);
// 		auto p0 = point< double >(
// 			(p0_y.x() + p0_x.x()) / 2, (p0_y.y() + p0_x.y()) / 2
// 		);
// 		auto p1 = y_calc(cp1);
// 		auto p2 = x_calc(cp2);
// 
// 		std::cout << "Points on x or y plane:" << std::endl;
// 		std::cout << p0_y << std::endl;
// 		std::cout << p0_x << '\n' << std::endl;
// 		std::cout << p0 << std::endl;
// 		std::cout << p1 << std::endl;
// 		std::cout << p2 << std::endl;
// 
// 		auto p_0 = make_col_vector< double >(3_R, {0, 0, p0.y()});
// 		auto p_1 = make_col_vector< double >(3_R, {0, p1.x(), p1.y()});
// 		auto p_2 = make_col_vector< double >(3_R, {p2.x(), 0, p2.y()});
// 
// 		std::cout << "Points in 3D:" << std::endl;
// 		std::cout << p_0 << std::endl;
// 		std::cout << p_1 << std::endl;
// 		std::cout << p_2 << std::endl;
// 
// 		auto p2d_0 = reduce(proj * expand(p_0));
// 		auto p2d_1 = reduce(proj * expand(p_1));
// 		auto p2d_2 = reduce(proj * expand(p_2));
// 
// 		std::cout << "Point on camera projected:" << std::endl;
// 		std::cout << p2d_0 << std::endl;
// 		std::cout << p2d_1 << std::endl;
// 		std::cout << p2d_2 << std::endl;
// 
// 		auto proj4x4 = make_square_matrix_by_function(4_D,
// 			[&proj](size_t x, size_t y)->double{
// 				if(y == 3){
// 					return 0;
// 				}
// 				return proj(x, y);
// 			});
// 
// 		{
// 			auto C4 = matrix_kernel(proj4x4);
// 
// 			auto C = reduce(C4);
// // 			C[2] = 0;
// 			std::cout << "C" << std::endl;
// 			std::cout << "  C4: " << C4 << std::endl;
// 			std::cout << "   C: " << C << std::endl;
// 			std::cout << "P*C4: " << proj * C4 << std::endl;
// 			std::cout << " P*C: " << proj * expand(C) << std::endl;
// 		}
// 
// 		auto cam_matrix = make_square_matrix_by_function(3_D,
// 			[&proj](size_t x, size_t y)->double{
// 				return proj(x, y);
// 			});
// 
// 		auto t = make_col_vector< double >(
// 			3_R, {proj(3, 0), proj(3, 1), proj(3, 2)}
// 		);
// 
// 		std::cout << "t" << std::endl;
// 		std::cout << "t: " << t << std::endl;
// 
// 		auto r = make_square_matrix< double >(3_D);
// 		auto k = make_square_matrix< double >(3_D);
// 
// 		std::tie(r, k) = mitrax::householder_transformation(cam_matrix);
// 
// 		std::cout << "RQ" << std::endl;
// 		std::cout << "cam: " << cam_matrix << std::endl;
// 		std::cout << "r*k: " << r * k << std::endl;
// 		std::cout << "  r: " << r << std::endl;
// 		std::cout << "  k: " << k << std::endl;
// 
// 		auto C = -inverse(r) * t;
// 		std::cout << "C = -R^{-1}t" << std::endl;
// 		std::cout << "   C: " << C << std::endl;
// 		std::cout << " P*C: " << proj * expand(C) << std::endl;
// 
// 		auto C3x4 = make_matrix_by_function(4_C, 3_R,
// 			[&C](size_t x, size_t y)->double{
// 				if(x == 3) return -C[y];
// 				return x == y ? 1 : 0;
// 			});
// 
// 		std::cout << "Projektion" << std::endl;
// 		std::cout << "     C3x4: " << C3x4 << std::endl;
// 		std::cout << "      r*k: " << (r * k) << std::endl;
// 		std::cout << "r*k*[I|C]: " << (r * k * C3x4) << std::endl;
// 		std::cout << "        P: " << proj << std::endl;
// 
// // 		r *= k(2, 2);
// // 		k /= k(2, 2);
// // 
// // 		std::cout << "Scale r and k" << std::endl;
// // 		std::cout << "        r: " << r << std::endl;
// // 		std::cout << "        k: " << k << std::endl;
// // 		std::cout << "      I|C: " << C3x4 << std::endl;
// // 		std::cout << "      r*k: " << (r * k) << std::endl;
// // 		std::cout << "r*k*[I|C]: " << (r * k * C3x4) << std::endl;
// 
// 		auto laser_plane = geometry3d::plane< double >(p_0, p_1, p_2);
// 
// 		auto g = laser_plane.general_form();
// 		auto n = laser_plane.point_normal_form();
// 
// 		auto to_3d = [&](auto p){
// 			auto krp = inverse(k * r) * expand(p);
// 
// 			auto factor =
// 				((-n.d - dot_product(g.n, C)) / dot_product(g.n, krp));
// 
// 			std::cout << factor << std::endl;
// 
// 			return factor * krp + C;
// 		};
// 
// 		auto p3d_0 = to_3d(p2d_0);
// 		auto p3d_1 = to_3d(p2d_1);
// 		auto p3d_2 = to_3d(p2d_2);
// 
// 		std::cout << "Points on laser plane:" << std::endl;
// 		std::cout << p3d_0 << std::endl;
// 		std::cout << p3d_1 << std::endl;
// 		std::cout << p3d_2 << std::endl;
	}


}
