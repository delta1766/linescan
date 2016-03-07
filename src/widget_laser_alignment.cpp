//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_laser_alignment.hpp>
#include <linescan/to_image.hpp>
#include <linescan/binarize.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/calc_top_distance_line.hpp>
#include <linescan/erode.hpp>
#include <linescan/point.hpp>


namespace linescan{


	namespace{


		std::pair< QImage, QImage > draw_laser_alignment(
			mitrax::raw_bitmap< std::uint8_t > const& bitmap
		){
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

			auto text = [&]{
				if(points.size() < 2) return QString("no line");

				auto line = fit_linear_function< double >(
					points.begin(), points.end()
				);

				painter.setPen(qRgb(255, 0, 0));
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

			return { to_image(bitmap), overlay };
		}


	}


	widget_laser_alignment::widget_laser_alignment(camera& cam):
		image_(cam)
	{
		image_.set_processor(&draw_laser_alignment);

		layout_.addWidget(&image_);
		setLayout(&layout_);
	}


}
