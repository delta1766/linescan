//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_calib__hpp_INCLUDED_
#define _linescan__widget_calib__hpp_INCLUDED_

#include "widget_processing_base.hpp"
#include "control_F9S_MCL3.hpp"
#include "calibration.hpp"
#include "circle.hpp"

#include <iostream>


namespace linescan{


	class widget_calib: public widget_processing_base{
	public:
		widget_calib(camera& cam, control_F9S_MCL3& mcl3);


		bool is_running()const;

		void stop();
		void start();


	protected:
		void hideEvent(QHideEvent* event);


	private:
		enum class step{
			align,
			calib_yz,
			calib_x,
			complete
		};

		struct xy_data{
			xy_data(circle const& c1, circle const& c2, double y, double Z):
				c1(c1), c2(c2), y(y), Z(Z) {}

			circle c1;
			circle c2;
			double y;
			double Z;
		};


		QImage draw_yz(
			std::vector< mitrax::point< double > > left_points,
			std::vector< mitrax::point< double > > right_points,
			std::vector< mitrax::point< double > > y_to_Z_points,
			double min_z, double max_z
		)const;

		void align_ready();
		void analyze_yz();
		void analyze_x();

		void reset();
		void set_running(bool is_running);

		void set_step(step s);


		step step_ = step::align;

		bool running_ = false;

		std::size_t save_count_line_ = 0;
		std::size_t exception_count_ = 0;
		std::array< std::int64_t, 3 > null_pos_{{0, 0, 0}};

		mitrax::raw_bitmap< std::uint8_t > bitmap_;
		std::vector< xy_data > circle_calib_;

		camera& cam_;
		control_F9S_MCL3& mcl3_;

		QRadioButton laser_line_;

		QLabel step_l_;
		QPushButton laser_start_;
		QLabel laser_auto_stop_l_;
		QCheckBox laser_auto_stop_;

		QTimer timer_;
	};


}


#endif
