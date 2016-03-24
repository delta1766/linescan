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
#include <linescan/calc_top_distance_line.hpp>

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

	mitrax::raw_bitmap< std::uint8_t > draw_top_distance_line(
		std::vector< mitrax::point< double > > const& line,
		std::size_t cols,
		std::size_t rows
	){
		auto image =
			mitrax::make_bitmap_by_default< std::uint8_t >(cols, rows);

		draw(image, line);

		return image;
	}

	mitrax::raw_bitmap< bool > draw_top_distance_line_student(
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
			calc_top_distance_line(bitmap, binarize_threshold, erode_value)
		);
	}

	void draw_align_text(QImage& overlay, QString const& text){
			QPainter painter(&overlay);
			painter.setRenderHint(QPainter::Antialiasing, true);

			painter.setPen(qRgb(0, 255, 0));
			QFont font = painter.font();
			font.setPixelSize(128);
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
		if(points.size() < 2){
			QImage overlay(
				dims.cols(), dims.rows(), QImage::Format_ARGB32
			);
			overlay.fill(0);

			draw_align_text(overlay, QObject::tr("no line"));

			return overlay;
		}

		return draw_line(dims, fit_polynom< 1 >(points));
	}

	QImage draw_line(
		mitrax::bitmap_dims_t const& dims,
		polynom< double, 1 > const& line
	){
		QImage overlay(
			dims.cols(), dims.rows(), QImage::Format_ARGB32
		);
		overlay.fill(0);

		{
			QPainter painter(&overlay);
			painter.setRenderHint(QPainter::Antialiasing, true);

			painter.setPen(QPen(QBrush(qRgb(255, 0, 0)), 3));
			painter.drawLine(
				0, line(0),
				overlay.width() - 1, line(overlay.width() - 1)
			);
		}

		auto angle = std::sin((line(100) - line(0)) / 100);
		draw_align_text(
			overlay,
			QString("%1°").arg(angle * 180 / M_PI, 0, 'f', 1)
		);

		return overlay;
	}

	QImage draw_circle_line(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap
	){
		try{
			return draw_circle_line(bitmap.dims(), find_calib_line(bitmap));
		}catch(std::exception const& error){
			std::cerr << "Exception: " << error.what() << std::endl;
		}catch(...){
			std::cerr << "Unknown exception" << std::endl;
		}

		QImage overlay(
			bitmap.cols(), bitmap.rows(), QImage::Format_ARGB32
		);
		overlay.fill(0);

		draw_align_text(overlay, QObject::tr("no circles"));

		return overlay;
	}

	QImage draw_circle_line(
		mitrax::bitmap_dims_t const& dims,
		std::array< circle, 2 > const& circles
	){
		using namespace mitrax::literals;

		auto c1 = circles[0];
		auto c2 = circles[1];

		double dx = c2.x() - c1.x();
		double dy = c2.y() - c1.y();
		if(dx == 0) dx = 1;

		auto m = dy / dx;
		auto a = c1.y() - m * c1.x();
		polynom< double, 1 > line(
			mitrax::make_col_vector< double >(2_R, {a, m})
		);

		auto overlay = draw_line(dims, line);

		{
			// draw circles in red
			QPainter painter(&overlay);
			painter.setRenderHint(QPainter::Antialiasing, true);

			painter.setPen(QPen(QBrush(Qt::red), 1));
			for(auto const& c: circles){
				painter.drawEllipse(
					QPointF(c.x(), c.y()), c.radius(), c.radius()
				);
			}
		}

		return overlay;
	}



}
