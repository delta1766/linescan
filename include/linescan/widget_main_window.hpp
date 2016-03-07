//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_main_window__hpp_INCLUDED_
#define _linescan__widget_main_window__hpp_INCLUDED_

#include "widget_camera_dock.hpp"
#include "widget_live_actions.hpp"
#include "widget_laser_alignment.hpp"
#include "control_F9S_MCL3.hpp"

#include <QtWidgets/QtWidgets>


namespace linescan{


	class widget_main_window: public QMainWindow{
	public:
		widget_main_window();


	private:
		control_F9S_MCL3 mcl3_;
		camera cam_;

		widget_camera_dock cam_dock_;

		QTabWidget tabs_;

		widget_live_actions live_actions_;
		widget_laser_alignment laser_alignment_;
	};


}


#endif
