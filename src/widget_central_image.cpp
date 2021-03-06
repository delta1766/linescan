//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_central_image.hpp>


namespace linescan{


	void widget_central_image::set_images(
		QImage const& image, QImage const& overlay
	){
		image_ = image;
		overlay_ = overlay;
		repaint();
	}

	void widget_central_image::set_overlay(QImage const& overlay){
		overlay_ = overlay;
		repaint();
	}

	void widget_central_image::paintEvent(QPaintEvent*){
		QPainter painter(this);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);

		auto draw = [this, &painter](QImage const& image){
			if(width() < image.width() || height() < image.height()){
				auto factor = std::min(
					float(width()) / image.width(),
					float(height()) / image.height()
				);

				painter.drawImage(
					QRectF(
						(width() - factor * image.width()) / 2,
						(height() - factor * image.height()) / 2,
						factor * image.width(),
						factor * image.height()
					), image
				);
			}else{
				painter.drawImage(
					(width() - image.width()) / 2,
					(height() - image.height()) / 2,
					image
				);
			}
		};

		draw(image_);
		draw(overlay_);
	}


}
