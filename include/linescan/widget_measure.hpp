//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_measure__hpp_INCLUDED_
#define _linescan__widget_measure__hpp_INCLUDED_

#include "widget_live_image.hpp"
#include "control_F9S_MCL3.hpp"

#include <QtWidgets/QtWidgets>

#include <boost/signals2.hpp>


namespace linescan{


	class widget_measure: public QWidget{
	public:
		widget_measure(camera& cam, control_F9S_MCL3& mcl3);

		~widget_measure();


		boost::signals2::signal< void(QString const&) > message;


	private:
		std::size_t save_count_ = 0;

		camera& cam_;
		control_F9S_MCL3& mcl3_;

		QHBoxLayout layout_;
		QGridLayout main_layout_;

		QPushButton save_;

		widget_live_image image_;

		QTimer timer_;
	};


}


#endif
