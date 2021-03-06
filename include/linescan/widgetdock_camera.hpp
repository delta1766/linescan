//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widgetdock_camera__hpp_INCLUDED_
#define _linescan__widgetdock_camera__hpp_INCLUDED_

#include "camera.hpp"

#include <QtWidgets>


namespace linescan{


	/// \brief Widget for camera settings
	class widgetdock_camera: public QDockWidget{
	public:
		/// \brief Constructor
		widgetdock_camera(camera& cam);


	private:
		void set_ranges();

		void set_pixelclock_ranges();
		void set_framerate_ranges();
		void set_exposure_ranges();
		void set_gain_ranges();


		camera& cam_;

		QWidget widget_;
		QGridLayout layout_;

		QLabel pixelclock_l_;
		QSpinBox pixelclock_v_;
		QSlider pixelclock_;
		QLabel pixelclock_ml_;
		QLabel pixelclock_xl_;
		QLabel pixelclock_il_;

		QLabel framerate_l_;
		QDoubleSpinBox framerate_v_;
		QSlider framerate_;
		QLabel framerate_ml_;
		QLabel framerate_xl_;
		QLabel framerate_il_;

		QLabel exposure_l_;
		QDoubleSpinBox exposure_v_;
		QSlider exposure_;
		QLabel exposure_ml_;
		QLabel exposure_xl_;
		QLabel exposure_il_;

		QLabel gain_l_;
		QSpinBox gain_v_;
		QSlider gain_;
		QLabel gain_ml_;
		QLabel gain_xl_;
		QLabel gain_il_;

		QLabel gain_boost_l_;
		QCheckBox gain_boost_;

		QHBoxLayout button_layout_;
		QPushButton default_light_;
		QPushButton min_light_;
		QPushButton max_light_;
	};


}


#endif
