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


	namespace{

		std::pair< QImage, QImage > standard_processor(
			mitrax::raw_bitmap< std::uint8_t > const& bitmap
		){
			return { to_image(bitmap), QImage() };
		}

	}

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
		cam_(cam),
		processor_(&standard_processor)
	{
		connect(&timer_, &QTimer::timeout, [this]{
			std::tie(image_, overlay_) = processor_(cam_.image());

			repaint();

			if(isVisible()) timer_.start(100);
		});
	}

	void live_image::set_processor(processor_type const& function){
		processor_ = function;
	}

	void live_image::reset_processor(){
		processor_ = &standard_processor;
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
