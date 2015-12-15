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


		void show_bitmap(mitrax::raw_bitmap< std::uint8_t > const& bitmap);


	private:
		control_F9S_MCL3 mcl3_;
		camera cam_;

		std::vector< std::vector< point< float > > > points_;

		std::array< double, 3 > intrinsic_parameters_;

		QGraphicsPixmapItem item_;
		QGraphicsView view_;
		QGraphicsScene scene_;

		QDockWidget dock_;
		QWidget dock_widget_;
		QHBoxLayout dock_layout_;

		QWidget main_dock_widget_;
		QHBoxLayout main_dock_layout_;
		QPushButton laser_align_;
		QPushButton calib_intrinsic_;
		QPushButton calib_extrinsic_;

		QWidget intrinsic_dock_widget_;
		QHBoxLayout intrinsic_dock_layout_;
		QPushButton intrinsic_get_;
		QPushButton intrinsic_ready_;

		QWidget extrinsic_dock_widget_;
		QHBoxLayout extrinsic_dock_layout_;
		QPushButton extrinsic_get_;

		QWidget laser_dock_widget_;
		QHBoxLayout laser_dock_layout_;
		QPushButton laser_ok_;
		QLabel laser_label_;

		QTimer timer_;
		QTimer laser_timer_;
	};


}


#endif
