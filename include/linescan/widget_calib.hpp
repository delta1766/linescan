//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 Benjamin Buch
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

#include <QtWidgets>

#include <boost/signals2.hpp>


namespace linescan{


	/// \brief Calibration widget
	class widget_calib: public QWidget{
	public:
		/// \brief Constructor
		widget_calib(camera& cam, control_F9S_MCL3& mcl3);

		/// \brief Destructor
		~widget_calib();


		/// \brief Signal for calibration has successfully completed
		boost::signals2::signal< void(calibration const&) > ready;


		/// \brief Set the distance between the midpoints of the calibration
		///        circles
		void set_target_distance_in_mm(double value);


		/// \brief Messages are shown in main window status bar
		boost::signals2::signal< void(QString const&) > message;


	protected:
		/// \brief Save MCL position as null position
		void showEvent(QShowEvent* event);

		/// \brief Call reset()
		void hideEvent(QHideEvent* event);


	private:
		/// \brief The 3 calibration steps
		enum class step{
			align_laser,
			align_target,
			calib_yz,
			complete
		};

		/// \brief Data of a single MCL position while calib_yz step
		struct xy_data{
			xy_data(circle const& c1, circle const& c2, double y, double Z):
				c1(c1), c2(c2), y(y), Z(Z) {}

			circle c1;
			circle c2;
			double y;
			double Z;
		};


		/// \brief Start calibration step calib_yz
		///
		/// Start the timer.
		void start();

		/// \brief Return MCL to null position and stop calib_yz timer
		void stop();


		/// \brief Draw calibration results
		QImage draw_yz(
			std::vector< mitrax::point< double > > left_points,
			std::vector< mitrax::point< double > > right_points,
			std::vector< mitrax::point< double > > y_to_Z_points,
			double min_z, double max_z
		)const;

		/// \brief Analyze all positions from calibration step calib_yz
		///
		/// Create the calibration object and fire the signal.
		void analyze_yz();

		/// \brief Restart the calibration process
		void reset();

		/// \brief Go to another step
		void set_step(step s);


		/// \brief The actual step
		step step_ = step::align_laser;

		/// \brief true, if calib_yz timer is running
		bool running_ = false;


		/// \brief Simulation counter
		std::size_t save_count_line_ = 0;

		/// \brief Count of back-to-back exceptions in calib_yz step
		std::size_t exception_count_ = 0;

		/// \brief MCL null position
		std::array< std::int64_t, 3 > null_pos_{{0, 0, 0}};


		/// \brief Minimum values of all calib_yz step positions
		mitrax::std_bitmap< std::uint8_t > bitmap_;

		/// \brief Data of all MCL position while calib_yz step
		std::vector< xy_data > circle_calib_;


		/// \brief Reference to camera
		camera& cam_;

		/// \brief Reference to MCL
		control_F9S_MCL3& mcl3_;


		QHBoxLayout layout_;
		QGridLayout main_layout_;

		QLabel step_l_;
		QPushButton laser_start_;
		QPushButton save_;

		widget_live_image image_;


		/// \brief calib_yz timer
		QTimer timer_;


		/// \brief Distance between the calibration circle midpoints in mm
		double target_distance_in_mm_ = 25;
	};


}


#endif
