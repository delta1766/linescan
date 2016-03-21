//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/draw.hpp>
#include <linescan/to_image.hpp>
#include <linescan/binarize.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/calc_top_distance_line.hpp>
#include <linescan/erode.hpp>
#include <linescan/point.hpp>

#include <QtGui/QPainter>


namespace linescan{


	void draw(
		mitrax::raw_bitmap< std::uint8_t >& image,
		point< double > const& point
	){
		auto x = static_cast< int >(point.x());
		auto y = static_cast< int >(point.y());

		auto dx = point.x() - x;
		auto dy = point.y() - y;

		auto draw = [&image](int x, int y, double v){
			if(
				x < 0 || y < 0 ||
				x >= static_cast< int >(image.cols()) ||
				y >= static_cast< int >(image.rows())
			) return;

			auto r = image(x, y) + v;
			image(x, y) = static_cast< std::uint8_t >(r > 255 ? 255 : r);
		};

		draw(x, y, 255 * (1 - dx) * (1 - dy));
		draw(x + 1, y, 255 * dx * (1 - dy));
		draw(x, y + 1, 255 * (1 - dx) * dy);
		draw(x + 1, y + 1, 255 * dx * dy);
	}

	void draw(
		mitrax::raw_bitmap< std::uint8_t >& image,
		vector< point< double > > const& line
	){
		for(std::size_t i = 0; i < line.size(); ++i){
			draw(image, line[i]);
		}
	}

	mitrax::raw_bitmap< std::uint8_t > draw_top_distance_line(
		vector< double > const& line,
		std::size_t cols,
		std::size_t rows
	){
		auto image =
			mitrax::make_matrix< std::uint8_t >(mitrax::dims(cols, rows));

		vector< point< double > > point_line;
		for(std::size_t i = 0; i < line.size(); ++i){
			if(line[i] == 0) continue;
			point_line.emplace_back(i, line[i]);
		}

		draw(image, point_line);
		return image;
	}

	mitrax::raw_bitmap< bool > draw_top_distance_line_student(
		vector< double > const& line,
		std::size_t cols,
		std::size_t rows
	){
		auto image = mitrax::make_matrix< bool >(mitrax::dims(cols, rows));

		for(std::size_t i = 0; i < line.size(); ++i){
			if(line[i] == 0) continue;
			image(i, static_cast< size_t >(line[i])) = true;
		}

		return image;
	}

	std::pair< QImage, QImage >
	draw_laser_alignment(mitrax::raw_bitmap< std::uint8_t >&& bitmap){
		auto binary = binarize(bitmap, std::uint8_t(255));
		binary = erode(binary, 3);

		auto top_distance_line = calc_top_distance_line(binary);

		std::vector< point< double > > points;
		for(std::size_t i = 0; i < top_distance_line.size(); ++i){
			if(top_distance_line[i] == 0) continue;
			points.emplace_back(i, top_distance_line[i]);
		}

		QImage overlay(
			bitmap.cols(), bitmap.rows(), QImage::Format_ARGB32
		);
		overlay.fill(0);

		QPainter painter(&overlay);
		painter.setRenderHint(QPainter::Antialiasing, true);

		auto text = [&]{
			if(points.size() < 2) return QString("no line");

			auto line = fit_linear_function< double >(
				points.begin(), points.end()
			);

			painter.setPen(QPen(QBrush(qRgb(255, 0, 0)), 3));
			painter.drawLine(
				0, line(0),
				overlay.width() - 1, line(overlay.width() - 1)
			);

			auto angle = std::sin((line(100) - line(0)) / 100);

			return QString("%1°").arg(angle * 180 / M_PI, 0, 'f', 1);
		}();

		painter.setPen(qRgb(0, 255, 0));
		QFont font = painter.font();
		font.setPixelSize(128);
		painter.setFont(font);
		painter.drawText(
			0, 0, overlay.width(), overlay.height() / 2,
			Qt::AlignCenter, text
		);

		return { to_image(std::move(bitmap)), overlay };
	}



}
