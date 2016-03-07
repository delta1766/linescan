//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_central_image__hpp_INCLUDED_
#define _linescan__widget_central_image__hpp_INCLUDED_

#include <QtWidgets/QtWidgets>


namespace linescan{


	class widget_central_image: public QMainWindow{
	public:
		void set_images(QImage const& image, QImage const& overlay);


	protected:
		virtual void paintEvent(QPaintEvent* event)override;


	private:
		QImage image_;
		QImage overlay_;
	};


}


#endif
