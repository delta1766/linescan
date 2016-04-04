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
#include "calibration.hpp"
#include "circlefind.hpp"

#include <iostream>


namespace linescan{


	class widget_calib_via_line: public widget_processing_base{
	public:
		widget_calib_via_line(
			camera& cam,
			control_F9S_MCL3& mcl3,
			std::function< void(laser_calibration const&) > set_laser_calib
		);


		bool is_running()const;

		void stop();
		void start();


	protected:
		void hideEvent(QHideEvent* event);


	private:
		struct laser_analyse_data{
			laser_analyse_data(double x1, double x2, double y_2d, double z_3d):
				x1(x1), x2(x2), y_2d(y_2d), z_3d(z_3d) {}

			double x1;
			double x2;
			double y_2d;
			double z_3d;
		};

		struct circle_analyse_data{
			circle_analyse_data(
				circle const& c1, circle const& c2, double y_2d, double z_3d
			):
				c1(c1), c2(c2), y_2d(y_2d), z_3d(z_3d) {}

			circle c1;
			circle c2;
			double y_2d;
			double z_3d;
		};

		void analyze_laser();
		void analyze_target();
		void reset();
		void set_running(bool is_running);

		enum class step{
			laser,
			target,
			complete
		} step_ = step::laser;

		void set_step(step s);

		camera& cam_;
		control_F9S_MCL3& mcl3_;
		std::function< void(laser_calibration const&) > set_laser_calib_;

		double height_;
		mitrax::raw_bitmap< std::uint8_t > bitmap_;
		std::vector< laser_analyse_data > laser_calib_;
		std::vector< circle_analyse_data > circle_calib_;
// 		std::vector< mitrax::point< double > > y_to_height_points_;
// 		std::vector< circle > last_circles_;
		std::array< mitrax::point< double >, 2 > target_differences_;

		std::size_t save_count_line_;

		QRadioButton laser_line_;

		QLabel step_l_;
		QPushButton laser_start_;
		QLabel laser_auto_stop_l_;
		QCheckBox laser_auto_stop_;

		bool running_;

		QTimer timer_;
	};


}


#endif
