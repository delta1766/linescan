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

#include "widget_live_image.hpp"
#include "control_F9S_MCL3.hpp"
#include "calibration.hpp"
#include "circle.hpp"

#include <QtWidgets/QtWidgets>

#include <boost/signals2.hpp>


namespace linescan{


	class widget_calib: public QWidget{
	public:
		widget_calib(camera& cam, control_F9S_MCL3& mcl3);

		~widget_calib();

		boost::signals2::signal< void(calibration const&) > ready;


	protected:
		void showEvent(QShowEvent* event);
		void hideEvent(QHideEvent* event);


	private:
		enum class step{
			align_laser,
			align_target,
			calib_yz,
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


		void stop();
		void start();


		QImage draw_yz(
			std::vector< mitrax::point< double > > left_points,
			std::vector< mitrax::point< double > > right_points,
			std::vector< mitrax::point< double > > y_to_Z_points,
			double min_z, double max_z
		)const;

		void analyze_yz();

		void reset();

		void set_step(step s);


		step step_ = step::align_laser;
		bool running_ = true;


		std::size_t save_count_line_ = 0;
		std::size_t exception_count_ = 0;
		std::array< std::int64_t, 3 > null_pos_{{0, 0, 0}};

		mitrax::raw_bitmap< std::uint8_t > bitmap_;
		std::vector< xy_data > circle_calib_;

		camera& cam_;
		control_F9S_MCL3& mcl3_;

		QHBoxLayout layout_;
		QGridLayout main_layout_;

		QLabel step_l_;
		QPushButton laser_start_;

		widget_live_image image_;

		QTimer timer_;


		double const target_distance_in_mm_ = 25;
	};


}


#endif
