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
#include "calibration.hpp"

#include <QtWidgets>

#include <boost/signals2.hpp>


namespace linescan{


	/// \brief Measurement widget
	class widget_measure: public QWidget{
	public:
		/// \brief Constructor
		widget_measure(camera& cam, control_F9S_MCL3& mcl3);

		/// \brief Destructor
		~widget_measure();


		/// \brief Set a calibration
		void set_calibration(calibration const& calib);


		/// \brief Messages are shown in main window status bar
		boost::signals2::signal< void(QString const&) > message;


	private:
		/// \brief Start measurement
		void start();

		/// \brief Stop measurement, move MCL to start position
		void stop();

		/// \brief Enable/disable control spin boxes
		void set_enabled(bool on);


		std::size_t image_save_count_ = 0;
		std::size_t measure_save_count_ = 0;
		std::size_t exception_count_ = 0;
		bool running_ = true;

		/// \brief 3D-coordinates
		std::vector< std::array< double, 3 > > points_;

		camera& cam_;
		control_F9S_MCL3& mcl3_;
		calibration calib_;

		QHBoxLayout layout_;
		QGridLayout main_layout_;

		QLabel y_l_;
		QLabel y_from_l_;
		QDoubleSpinBox y_from_;
		QLabel y_to_l_;
		QDoubleSpinBox y_to_;
		QLabel y_step_l_;
		QDoubleSpinBox y_step_;

		QLabel x_l_;
		QLabel x_from_l_;
		QDoubleSpinBox x_from_;
		QLabel x_to_l_;
		QDoubleSpinBox x_to_;
		QLabel x_step_l_;
		QDoubleSpinBox x_step_;

		QPushButton start_;
		QPushButton save_;

		widget_live_image image_;

		QTimer timer_;
	};


}


#endif
