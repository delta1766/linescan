//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__main_window__hpp_INCLUDED_
#define _linescan__main_window__hpp_INCLUDED_

#include "point.hpp"
#include "camera.hpp"
#include "control_F9S_MCL3.hpp"

#include <QtWidgets/QtWidgets>


#ifdef HARDWARE
#ifndef MCL
#define MCL
#endif
#ifndef CAM
#define CAM
#endif
#endif

#if defined(MCL) && defined(CAM) && !defined(HARDWARE)
#define HARDWARE
#endif


namespace linescan{


	class main_window: public QMainWindow{
	public:
		main_window();

		~main_window();


		void laser_live();

		void show_bitmap(mitrax::raw_bitmap< std::uint8_t > const& bitmap);

		void show_main_image();

		void show_process_image();


	private:
		control_F9S_MCL3 mcl3_;
		camera cam_;

		std::vector< std::vector< point< float > > > points_;
		std::vector< point< double > > points_3d_;

		std::array< double, 3 > camera_matrix_parameter_;
		std::array< double, 8 > distortion_coefficients_;

		std::array< double, 3 > rotation_vector_;
		std::array< double, 3 > translation_vector_;

		QGraphicsPixmapItem item_;
		QGraphicsView view_;
		QGraphicsScene scene_;

		QDockWidget dock_;
		QWidget dock_widget_;
		QHBoxLayout dock_layout_;

		QWidget main_dock_widget_;
		QHBoxLayout main_dock_layout_;
		QPushButton align_laser_;
		QPushButton calib_intrinsic_;
		QPushButton calib_extrinsic_;

		QWidget intrinsic_dock_widget_;
		QHBoxLayout intrinsic_dock_layout_;
		QLabel intrinsic_label_;
		QPushButton intrinsic_get_;
		QPushButton intrinsic_ready_;

		QWidget extrinsic_dock_widget_;
		QHBoxLayout extrinsic_dock_layout_;
		QLabel extrinsic_label_;
		QPushButton extrinsic_get_;
		QPushButton extrinsic_ready_;

		QWidget laser_dock_widget_;
		QHBoxLayout laser_dock_layout_;
		QLabel laser_label_;
		QPushButton laser_ok_;

		QTimer timer_;
		QTimer laser_timer_;
	};


}


#endif
