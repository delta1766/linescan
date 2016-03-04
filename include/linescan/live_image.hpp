//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__live_image__hpp_INCLUDED_
#define _linescan__live_image__hpp_INCLUDED_

#include <linescan/camera.hpp>

#include <QtWidgets/QtWidgets>


namespace linescan{


	class live_image: public QMainWindow{
	public:
		live_image(camera& cam);


	protected:
		virtual void showEvent(QShowEvent* event)override;

		virtual void hideEvent(QHideEvent* event)override;

		virtual void paintEvent(QPaintEvent* event)override;


	private:
		camera& cam_;

		QImage image_;

		QTimer timer_;
	};


}


#endif
