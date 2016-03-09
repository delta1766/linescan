//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_calib__hpp_INCLUDED_
#define _linescan__widget_calib__hpp_INCLUDED_

#include "widget_live_image.hpp"

#include <QtWidgets/QtWidgets>


namespace linescan{


	class widget_calib: public QWidget{
	public:
		widget_calib(camera& cam);


	private:
		QHBoxLayout layout_;

		widget_live_image image_;
		QToolBox tabs_;

		QWidget intrinsics_;
		QWidget extrinsics_;
		QWidget movements_;

		QVBoxLayout intrinsic_layout_;
		QListWidget intrinsic_images_;
		QPushButton intrinsic_button_;
	};


}


#endif
