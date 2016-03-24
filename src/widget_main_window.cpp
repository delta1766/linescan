//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_main_window.hpp>


namespace linescan{


	widget_main_window::widget_main_window():
		mcl3_("/dev/ttyUSB0"),
		cam_(0),
		cam_dock_w_(cam_),
		live_actions_w_(cam_, [this](QString const& message){
			statusBar()->showMessage(message, 5000);
		}),
		laser_alignment_w_(cam_),
		calib_w_(cam_),
		calib_via_line_w_(cam_, mcl3_, [this](laser_calibration const& calib){
			calib_ = calib;

			auto cam_y_to_z_map = calib_.cam_y_to_z_map();
			auto left_limit = calib_.left_limit();
			auto right_limit = calib_.right_limit();

			settings_.beginGroup("calibration");
			settings_.beginGroup("laser");
			settings_.beginGroup("cam_y_to_z_map");
			settings_.setValue("a", cam_y_to_z_map[3]);
			settings_.setValue("b", cam_y_to_z_map[2]);
			settings_.setValue("c", cam_y_to_z_map[1]);
			settings_.setValue("d", cam_y_to_z_map[0]);
			settings_.endGroup();
			settings_.beginGroup("left_limit");
			settings_.setValue("a", left_limit[1]);
			settings_.setValue("b", left_limit[0]);
			settings_.endGroup();
			settings_.beginGroup("right_limit");
			settings_.setValue("a", right_limit[1]);
			settings_.setValue("b", right_limit[0]);
			settings_.endGroup();
			settings_.endGroup();
			settings_.endGroup();
			settings_.sync();
		})
	{
		addDockWidget(Qt::TopDockWidgetArea, &cam_dock_w_);

		tabs_w_.addTab(&live_actions_w_, tr("Live"));
		tabs_w_.addTab(&laser_alignment_w_, tr("Laser align"));
		tabs_w_.addTab(&calib_w_, tr("Calibration"));
		tabs_w_.addTab(&calib_via_line_w_, tr("Calibration via line"));

		statusBar();

		setCentralWidget(&tabs_w_);

		auto get = [this](auto const& name){
			bool ok = false;
			double value = settings_.value(name).toDouble(&ok);
			return std::make_pair(value, ok);
		};

		settings_.beginGroup("calibration");
		settings_.beginGroup("laser");
		settings_.beginGroup("cam_y_to_z_map");
		auto cam_y_to_z_map_a = get("a");
		auto cam_y_to_z_map_b = get("b");
		auto cam_y_to_z_map_c = get("c");
		auto cam_y_to_z_map_d = get("d");
		settings_.endGroup();
		settings_.beginGroup("left_limit");
		auto laser_left_limit_a = get("a");
		auto laser_left_limit_b = get("b");
		settings_.endGroup();
		settings_.beginGroup("right_limit");
		auto laser_right_limit_a = get("a");
		auto laser_right_limit_b = get("b");
		settings_.endGroup();
		settings_.endGroup();
		settings_.endGroup();

		for(auto const& pair: {
			cam_y_to_z_map_a,
			cam_y_to_z_map_b,
			cam_y_to_z_map_c,
			cam_y_to_z_map_d,
			laser_left_limit_a,
			laser_left_limit_b,
			laser_right_limit_a,
			laser_right_limit_b
		}) if(!pair.second) return;

		using namespace mitrax::literals;

		polynom< double, 3 > cam_y_to_z_map(
			mitrax::make_col_vector< double >(4_R, {
				cam_y_to_z_map_d.first,
				cam_y_to_z_map_c.first,
				cam_y_to_z_map_b.first,
				cam_y_to_z_map_a.first
			}));

		polynom< double, 1 > laser_left_limit(
			mitrax::make_col_vector< double >(2_R, {
				laser_left_limit_b.first,
				laser_left_limit_a.first
			}));

		polynom< double, 1 > laser_right_limit(
			mitrax::make_col_vector< double >(2_R, {
				laser_right_limit_b.first,
				laser_right_limit_a.first
			}));

		calib_.set(cam_y_to_z_map, laser_left_limit, laser_right_limit);
	}


}
