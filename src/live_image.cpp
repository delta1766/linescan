//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/live_image.hpp>


namespace linescan{


	QImage to_image(mitrax::raw_bitmap< std::uint8_t > const& bitmap){
		QImage image(
			bitmap.cols(), bitmap.rows(),
			QImage::Format_Grayscale8
		);

		std::size_t cols = bitmap.cols();
		for(std::size_t y = 0; y < bitmap.rows(); ++y){
			std::copy(
				bitmap.impl().data().data() + cols * y,
				bitmap.impl().data().data() + cols * (1 + y),
				image.bits() + image.bytesPerLine() * y
			);
		}

		return image;
	}

	live_image::live_image(camera& cam):
		cam_(cam)
	{
		connect(&timer_, &QTimer::timeout, [this]{
			image_ = to_image(cam_.image());

			repaint();

			if(isVisible()) timer_.start(100);
		});
	}

	void live_image::showEvent(QShowEvent* event){
		QWidget::showEvent(event);
		timer_.start(0);
	}

	void live_image::hideEvent(QHideEvent* event){
		QWidget::hideEvent(event);
		timer_.stop();
	}

	void live_image::paintEvent(QPaintEvent*){
		QPainter painter(this);

		if(width() < image_.width() || height() < image_.height()){
			auto factor = std::min(
				float(width()) / image_.width(),
				float(height()) / image_.height()
			);

			painter.drawImage(
				QRectF(
					(width() - factor * image_.width()) / 2,
					(height() - factor * image_.height()) / 2,
					factor * image_.width(),
					factor * image_.height()
				), image_
			);
		}else{
			painter.drawImage(
				(width() - image_.width()) / 2,
				(height() - image_.height()) / 2,
				image_
			);
		}
	}


}
