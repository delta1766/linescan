//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/align_laser.hpp>

#include <linescan/to_pixmap.hpp>
#include <linescan/load.hpp>
#include <linescan/binarize.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/calc_top_distance_line.hpp>
#include <linescan/erode.hpp>
#include <linescan/point.hpp>

#include <QtGui/QPainter>

#include <cmath>


namespace linescan{


	std::tuple< QString, QPixmap > align_laser(camera& cam){
#ifdef CAM
		auto bitmap = cam.image();
#else
		(void)cam;
		auto bitmap = load("simulation/real2_laser.png");
#endif

		auto binary = binarize(bitmap, std::uint8_t(255));
		binary = erode(binary, 3);

		auto top_distance_line = calc_top_distance_line(binary);

		std::vector< point< double > > points;
		for(std::size_t i = 0; i < top_distance_line.size(); ++i){
			if(top_distance_line[i] == 0) continue;
			points.emplace_back(i, top_distance_line[i]);
		}

		auto pixmap = to_pixmap(bitmap);

		if(points.size() < 2) return {"no line", pixmap};

		auto line = fit_linear_function< double >(
			points.begin(), points.end()
		);

		auto angle = std::sin((line(100) - line(0)) / 100);
		auto angle_text = QString("%1°").arg(angle * 180 / M_PI, 0, 'f', 1);

		QPainter painter(&pixmap);

		painter.setPen(qRgb(0, 255, 0));
		QFont font = painter.font();
		font.setPixelSize(128);
		painter.setFont(font);
		painter.drawText(
			0, 0, pixmap.width(), pixmap.height() / 2,
			Qt::AlignCenter, angle_text
		);

		painter.setPen(qRgb(255, 0, 0));
		painter.drawLine(
			0, line(0),
			pixmap.width() - 1, line(pixmap.width() - 1)
		);

		return {angle_text, pixmap};
	}


}
