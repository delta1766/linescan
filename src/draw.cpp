//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/draw.hpp>
#include <linescan/processing.hpp>
#include <linescan/polynom.hpp>
#include <linescan/calc_laser_line.hpp>

#include <QtGui/QPainter>

#include <iostream>


namespace linescan{


	void draw(
		mitrax::raw_bitmap< std::uint8_t >& image,
		mitrax::point< double > const& point
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
		std::vector< mitrax::point< double > > const& line
	){
		for(std::size_t i = 0; i < line.size(); ++i){
			draw(image, line[i]);
		}
	}

	mitrax::raw_bitmap< std::uint8_t > draw_laser_line(
		std::vector< mitrax::point< double > > const& line,
		std::size_t cols,
		std::size_t rows
	){
		auto image =
			mitrax::make_bitmap_by_default< std::uint8_t >(cols, rows);

		draw(image, line);

		return image;
	}

	mitrax::raw_bitmap< bool > draw_laser_line_student(
		std::vector< mitrax::point< double > > const& line,
		std::size_t cols,
		std::size_t rows
	){
		auto image = mitrax::make_bitmap_by_default< bool >(cols, rows);

		for(auto const& p: line){
			image(
				static_cast< size_t >(p.x() + 0.5),
				static_cast< size_t >(p.y() + 0.5)
			) = true;
		}

		return image;
	}

	QImage draw_laser_alignment(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		std::uint8_t binarize_threshold,
		std::size_t erode_value
	){
		return draw_laser_alignment(
			bitmap.dims(),
			calc_laser_line(bitmap, binarize_threshold, erode_value)
		);
	}

	void draw_align_text(
		QImage& overlay, QString const& text, std::size_t font_size
	){
		QPainter painter(&overlay);
		painter.setRenderHint(QPainter::Antialiasing, true);

		painter.setPen(qRgb(0, 255, 0));
		QFont font = painter.font();
		font.setPixelSize(font_size);
		painter.setFont(font);

		painter.drawText(
			0, 0, overlay.width(), overlay.height() / 2,
			Qt::AlignCenter, text
		);
	}

	QImage draw_laser_alignment(
		mitrax::bitmap_dims_t const& dims,
		std::vector< mitrax::point< double > > const& points
	){
		QImage overlay(
			dims.cols(), dims.rows(), QImage::Format_ARGB32
		);
		overlay.fill(0);

		if(points.size() < 2){
			draw_align_text(overlay, QObject::tr("no line"), 128);

			return overlay;
		}

		draw_line(overlay, fit_polynom< 1 >(points));

		return overlay;
	}

	void draw_line(QImage& overlay, polynom< double, 1 > const& line){
		auto angle = std::sin((line(100) - line(0)) / 100);
		draw_align_text(
			overlay,
			QString("%1°").arg(angle * 180 / M_PI, 0, 'f', 1),
			128
		);

		QPainter painter(&overlay);
		painter.setRenderHint(QPainter::Antialiasing, true);

		painter.setPen(QPen(QBrush(qRgb(255, 0, 0)), 3));
		painter.drawLine(
			0, line(0),
			overlay.width() - 1, line(overlay.width() - 1)
		);
	}

	QImage draw_line(
		mitrax::bitmap_dims_t const& dims,
		polynom< double, 1 > const& line
	){
		QImage overlay(dims.cols(), dims.rows(), QImage::Format_ARGB32);
		overlay.fill(0);

		draw_line(overlay, line);

		return overlay;
	}

	QImage draw_circles(QImage& overlay, std::vector< circle > const& circles){
		QPainter painter(&overlay);
		painter.setRenderHint(QPainter::Antialiasing, true);

		painter.setPen(QPen(QBrush(Qt::red), 1));
		for(auto const& c: circles){
			painter.drawEllipse(
				QPointF(c.x(), c.y()), c.radius(), c.radius()
			);
		}

		return overlay;
	}

	QImage draw_circle_line(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		std::vector< circle > const& circles
	){
		QImage overlay(
			bitmap.cols(), bitmap.rows(), QImage::Format_ARGB32
		);
		overlay.fill(0);

		draw_circles(overlay, circles);

		if(circles.size() != 2){
			draw_align_text(overlay, QObject::tr("not two circles found"), 64);
			return overlay;
		}

		if(circles[1].x() == circles[0].x()){
			draw_align_text(
				overlay,
				QObject::tr("can not make linear function"),
				64
			);
			return overlay;
		}

		auto line = to_polynom(to_point(circles[0]), to_point(circles[1]));

		draw_line(overlay, line);

		return overlay;
	}

	QImage draw_circle_line(mitrax::raw_bitmap< std::uint8_t > const& bitmap){
		return draw_circle_line(bitmap, find_calib_circles(bitmap));
	}




}
