//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_camera_dock__hpp_INCLUDED_
#define _linescan__widget_camera_dock__hpp_INCLUDED_

#include <linescan/camera.hpp>

#include <QtWidgets/QtWidgets>


namespace linescan{


	class widget_camera_dock: public QDockWidget{
	public:
		widget_camera_dock(camera& cam);

	private:
		void set_ranges();


		camera& cam_;


		QWidget widget_;
		QGridLayout layout_;

		QLabel pixelclock_l_;
		QSpinBox pixelclock_v_;
		QSlider pixelclock_;

		QLabel framerate_l_;
		QDoubleSpinBox framerate_v_;
		QSlider framerate_;

		QLabel exposure_l_;
		QDoubleSpinBox exposure_v_;
		QSlider exposure_;

		QLabel gain_l_;
		QSpinBox gain_v_;
		QSlider gain_;

		QLabel gain_boost_l_;
		QCheckBox gain_boost_;
	};


}


#endif
