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

#include "point.hpp"
#include "widget_camera_dock.hpp"
#include "widget_live_image.hpp"
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


	class widget_main_window: public QMainWindow{
	public:
		widget_main_window();

		~widget_main_window();


// 		void laser_live();
// 
// 		void show_bitmap(mitrax::raw_bitmap< std::uint8_t > const& bitmap);
// 
// 		void show_main_image();
// 
// 		void show_process_image();


	private:
// 		enum class dock{
// 			main,
// 			align_laser,
// 			calib_extrinsic,
// 			calib_intrinsic,
// 			calib_laser
// 		};

// 		void show_dock(dock const& d);

		control_F9S_MCL3 mcl3_;
		camera cam_;

// 		std::vector< std::vector< point< float > > > points_;
// 		std::vector< point< double > > points_3d_;
// 
// 		std::array< double, 3 > camera_matrix_parameter_;
// 		std::array< double, 8 > distortion_coefficients_;
// 
// 		std::array< double, 3 > rotation_vector_;
// 		std::array< double, 3 > translation_vector_;

// 		QGraphicsPixmapItem item_;
// 		QGraphicsView view_;
// 		QGraphicsScene scene_;
// 
// 		QDockWidget dock_;
// 		QWidget dock_widget_;
// 		QVBoxLayout dock_layout_;
// 
// 		QWidget main_dock_widget_;
// 		QVBoxLayout main_dock_layout_;
// 		QPushButton align_laser_;
// 		QPushButton calib_intrinsic_;
// 		QPushButton calib_extrinsic_;
// 		QPushButton calib_laser_;
// 
// 		QWidget intrinsic_dock_widget_;
// 		QVBoxLayout intrinsic_dock_layout_;
// 		QLabel intrinsic_label_;
// 		QPushButton intrinsic_get_;
// 		QPushButton intrinsic_ready_;
// 
// 		QWidget extrinsic_dock_widget_;
// 		QVBoxLayout extrinsic_dock_layout_;
// 		QLabel extrinsic_label_;
// 		QPushButton extrinsic_get_;
// 		QPushButton extrinsic_ready_;
// 
// 		QWidget calib_laser_dock_widget_;
// 		QVBoxLayout calib_laser_dock_layout_;
// 		QLabel calib_laser_label_;
// 		QPushButton calib_laser_ok_;
// 
// 		QWidget laser_dock_widget_;
// 		QVBoxLayout laser_dock_layout_;
// 		QLabel laser_label_;
// 		QPushButton laser_ok_;

		widget_camera_dock cam_dock_;
		widget_live_image live_;

// 		QTimer timer_;
// 		QTimer laser_timer_;
	};


}


#endif
