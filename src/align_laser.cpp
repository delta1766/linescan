//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/align_laser.hpp>

#include <linescan/to_image.hpp>
#include <linescan/load.hpp>
#include <linescan/binarize.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/calc_top_distance_line.hpp>
#include <linescan/erode.hpp>
#include <linescan/point.hpp>

#include <QtGui/QPainter>

#include <cmath>


namespace linescan{


	std::tuple< QString, QImage > align_laser(camera& cam){
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

		auto image = to_image(bitmap);

		if(points.size() < 2) return {"no line", image};

		auto line = fit_linear_function< double >(
			points.begin(), points.end()
		);

		auto angle = std::sin((line(100) - line(0)) / 100);
		auto angle_text = QString("%1°").arg(angle * 180 / M_PI, 0, 'f', 1);

		QPainter painter(&image);

		painter.setPen(qRgb(0, 255, 0));
		QFont font = painter.font();
		font.setPixelSize(128);
		painter.setFont(font);
		painter.drawText(
			0, 0, image.width(), image.height() / 2,
			Qt::AlignCenter, angle_text
		);

		painter.setPen(qRgb(255, 0, 0));
		painter.drawLine(
			0, line(0),
			image.width() - 1, line(image.width() - 1)
		);

		return {angle_text, image};
	}


}
