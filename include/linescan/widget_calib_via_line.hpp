//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_calib_via_line__hpp_INCLUDED_
#define _linescan__widget_calib_via_line__hpp_INCLUDED_

#include "widget_processing_base.hpp"
#include "control_F9S_MCL3.hpp"
#include "polynom.hpp"

#include <iostream>


namespace linescan{


	class widget_calib_via_line: public widget_processing_base{
	public:
		widget_calib_via_line(camera& cam, control_F9S_MCL3& mcl3);


		bool is_running()const;

		void stop();
		void start();


	protected:
		void hideEvent(QHideEvent* event);


	private:
		void analyze();
		void set_running(bool is_running);

		camera& cam_;
		control_F9S_MCL3& mcl3_;

		mitrax::raw_bitmap< std::uint8_t > bitmap_;
		double height_;
		std::vector< mitrax::point< double > > y_to_height_points_;

		std::size_t save_count_line_;

		polynom< double, 3 > y_to_height_;
		polynom< double, 1 > left_laser_line_;
		polynom< double, 1 > right_laser_line_;

		QRadioButton line_;

		QPushButton laser_start_;
		QLabel laser_auto_stop_l_;
		QCheckBox laser_auto_stop_;

		bool running_;

		QTimer timer_;
	};


}


#endif
