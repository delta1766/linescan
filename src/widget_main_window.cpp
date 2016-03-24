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
		cam_dock_w_(cam_),
		live_actions_w_(cam_, [this](QString const& message){
			statusBar()->showMessage(message, 5000);
		}),
		laser_alignment_w_(cam_),
		calib_w_(cam_),
		calib_via_line_w_(cam_, mcl3_, [this](laser_calibration const& calib){
			calib_ = calib;
		})
	{
		addDockWidget(Qt::TopDockWidgetArea, &cam_dock_w_);

		tabs_w_.addTab(&live_actions_w_, tr("Live"));
		tabs_w_.addTab(&laser_alignment_w_, tr("Laser align"));
		tabs_w_.addTab(&calib_w_, tr("Calibration"));
		tabs_w_.addTab(&calib_via_line_w_, tr("Calibration via line"));

		statusBar();

		setCentralWidget(&tabs_w_);
	}


}
