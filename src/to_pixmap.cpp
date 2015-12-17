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
			bitmap.impl().data().data(),
			bitmap.cols(), bitmap.rows(),
			QImage::Format_Grayscale8
		);

		return QPixmap::fromImage(image);
	}


}
