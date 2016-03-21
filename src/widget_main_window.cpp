//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_main_window.hpp>


namespace linescan{


	widget_main_window::widget_main_window():
		mcl3_("/dev/ttyUSB0"),
		cam_(0),
		cam_dock_(cam_),
		live_actions_(cam_, [this](QString const& message){
			statusBar()->showMessage(message, 5000);
		}),
		laser_alignment_(cam_),
		calib_(cam_),
		calib_via_line_(cam_, mcl3_)
	{
		addDockWidget(Qt::TopDockWidgetArea, &cam_dock_);

		tabs_.addTab(&live_actions_, tr("Live"));
		tabs_.addTab(&laser_alignment_, tr("Laser align"));
		tabs_.addTab(&calib_, tr("Calibration"));
		tabs_.addTab(&calib_via_line_, tr("Calibration via line"));

		statusBar();

		setCentralWidget(&tabs_);
	}


}
