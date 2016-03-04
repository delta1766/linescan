//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/to_pixmap.hpp>


namespace linescan{


	QPixmap to_pixmap(mitrax::raw_bitmap< std::uint8_t > const& bitmap){
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

		return QPixmap::fromImage(image);
	}


}
