//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/calib_laser.hpp>

#include <linescan/ref3d.hpp>
#include <linescan/to_image.hpp>
#include <linescan/load.hpp>
#include <linescan/binarize.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/calc_top_distance_line.hpp>
#include <linescan/calc_calibration_lines.hpp>
#include <linescan/erode.hpp>

#include <mitrax/geometry3d.hpp>
#include <mitrax/gaussian_elimination.hpp>
#include <mitrax/operator.hpp>

#include <QtGui/QPainter>


namespace linescan{


	QImage calib_laser_image(
		camera& cam,
		std::vector< point< double > > const& points
	){
		using namespace mitrax;
		using namespace mitrax::literals;

#ifdef CAM
		auto bitmap = cam.image();
#else
		(void)cam;
		auto bitmap = load("simulation/real2_laser.png");
#endif
		try{
			auto binary = binarize(bitmap, std::uint8_t(255));

			binary = erode(binary, 3, false);

			auto line = calc_top_distance_line(binary);

			auto lines = calc_calibration_lines(line, 15);
			auto const& line_points_1 = lines.first;
			auto const& line_points_2 = lines.second;

			auto line1 = fit_linear_function< double >(
				line_points_1.begin(), line_points_1.end()
			);

			auto line2 = fit_linear_function< double >(
				line_points_2.begin(), line_points_2.end()
			);

			auto x = intersection(line1, line2);

			auto image = to_image(bitmap);
			QPainter painter(&image);

			painter.setPen(qRgb(255, 0, 0));
			painter.drawLine(0, line1(0), x, line1(x));
			painter.drawLine(
				x, line2(x), image.width() - 1, line2(image.width() - 1)
			);

			painter.setPen(qRgb(0, 255, 0));
			auto font = painter.font();
			font.setPixelSize(14);
			painter.setFont(font);

			std::size_t i = 1;
			for(auto const& p: points){
				QRect rect(p.x() - 10, p.y() - 10, 20, 20);
				painter.drawEllipse(rect);
				painter.drawText(rect, Qt::AlignCenter, QString("%1").arg(i));
				++i;
			}

			return image;
		}catch(std::exception const& error){
			auto image = to_image(bitmap);
			QPainter painter(&image);

			painter.setPen(qRgb(0, 255, 0));
			auto font = painter.font();
			font.setPixelSize(14);
			painter.setFont(font);
			painter.drawText(
				0, 0, image.width(), image.height() / 2,
				Qt::AlignCenter, error.what()
			);

			return image;
		}
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


}
