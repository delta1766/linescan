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

#include "widgetdock_mcl.hpp"
#include "widgetdock_camera.hpp"
#include "widgetdock_calc_laser_line.hpp"
#include "widget_measure.hpp"
#include "widget_calib.hpp"

#include <QtWidgets>


namespace linescan{


	/// \brief The main window
	class widget_main_window: public QMainWindow{
		Q_OBJECT
	public:
		/// \brief Constructor
		widget_main_window();


	private:
		QSettings settings_;

		control_F9S_MCL3 mcl3_;
		camera cam_;

		widgetdock_mcl mcl_dock_w_;
		widgetdock_camera cam_dock_w_;
		widgetdock_calc_laser_line laser_dock_w_;

		QTabWidget tabs_w_;

		widget_measure measure_w_;
		widget_calib calib_w_;
	};


}


#endif
