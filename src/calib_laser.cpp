//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/calib_laser.hpp>

#include <linescan/to_pixmap.hpp>
#include <linescan/load.hpp>
#include <linescan/binarize.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/calc_top_distance_line.hpp>
#include <linescan/calc_calibration_lines.hpp>
#include <linescan/erode.hpp>

#include <mitrax/geometry3d.hpp>

#include <QtGui/QPainter>


namespace linescan{


	QPixmap calib_laser_pixmap(
		camera& cam,
		std::vector< point< double > > const& points
	){
		using namespace mitrax;
		using namespace mitrax::literals;

#ifdef CAM
		auto image = cam.image();
#else
		(void)cam;
		auto image = load("simulation/real2_laser.png");
#endif
		try{
			auto binary = binarize(image, std::uint8_t(255));

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

			QPixmap pixmap = to_pixmap(image);
			QPainter painter(&pixmap);

			painter.setPen(qRgb(255, 0, 0));
			painter.drawLine(0, line1(0), x, line1(x));
			painter.drawLine(
				x, line2(x), pixmap.width() - 1, line2(pixmap.width() - 1)
			);

			painter.setPen(qRgb(0, 255, 0));
			QFont font = painter.font();
			font.setPixelSize(14);
			painter.setFont(font);

			std::size_t i = 1;
			for(auto const& p: points){
				QRect rect(p.x() - 10, p.y() - 10, 20, 20);
				painter.drawEllipse(rect);
				painter.drawText(rect, Qt::AlignCenter, QString("%1").arg(i));
				++i;
			}

			return pixmap;
		}catch(std::exception const& error){
			QPixmap pixmap = to_pixmap(image);
			QPainter painter(&pixmap);

			painter.setPen(qRgb(0, 255, 0));
			QFont font = painter.font();
			font.setPixelSize(14);
			painter.setFont(font);
			painter.drawText(
				0, 0, pixmap.width(), pixmap.height() / 2,
				Qt::AlignCenter, error.what()
			);

			return pixmap;
		}
	}


}
