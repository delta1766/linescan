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
		mcl_dock_w_(mcl3_),
		cam_dock_w_(cam_),
		measure_w_(cam_, mcl3_),
		calib_w_(cam_, mcl3_)
	{
		addDockWidget(Qt::TopDockWidgetArea, &cam_dock_w_);
		addDockWidget(Qt::LeftDockWidgetArea, &laser_dock_w_);
		addDockWidget(Qt::RightDockWidgetArea, &mcl_dock_w_);

		mcl_dock_w_.setAllowedAreas(
			Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea
		);

		cam_dock_w_.setAllowedAreas(
			Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea
		);

		laser_dock_w_.setAllowedAreas(
			Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea
		);

		tabs_w_.addTab(&measure_w_, tr("Measure"));
		tabs_w_.addTab(&calib_w_, tr("Calibration"));

		statusBar();

		setCentralWidget(&tabs_w_);


		measure_w_.message.connect([this](QString const& message){
			statusBar()->showMessage(message, 5000);
		});

		calib_w_.ready.connect([this](calibration const& calib){
			auto y_to_Z = calib.y_to_Z();
			auto y_to_X_null = calib.y_to_X_null();
			auto dx_to_dX = calib.dx_to_dX();

			settings_.beginGroup("calibration");
			settings_.beginGroup("laser");
			settings_.beginGroup("y_to_Z");
			settings_.setValue("a", y_to_Z[3]);
			settings_.setValue("b", y_to_Z[2]);
			settings_.setValue("c", y_to_Z[1]);
			settings_.setValue("d", y_to_Z[0]);
			settings_.endGroup();
			settings_.beginGroup("y_to_X_null");
			settings_.setValue("a", y_to_X_null[1]);
			settings_.setValue("b", y_to_X_null[0]);
			settings_.endGroup();
			settings_.beginGroup("dx_to_dX");
			settings_.setValue("a", dx_to_dX[1]);
			settings_.setValue("b", dx_to_dX[0]);
			settings_.endGroup();
			settings_.endGroup();
			settings_.endGroup();
			settings_.sync();

			measure_w_.set_calibration(calib);
		});


		auto get = [this](auto const& name){
			bool ok = false;
			double value = settings_.value(name).toDouble(&ok);
			return std::make_pair(value, ok);
		};

		settings_.beginGroup("calibration");
		settings_.beginGroup("laser");
		settings_.beginGroup("y_to_Z");
		auto y_to_Z_a = get("a");
		auto y_to_Z_b = get("b");
		auto y_to_Z_c = get("c");
		auto y_to_Z_d = get("d");
		settings_.endGroup();
		settings_.beginGroup("y_to_X_null");
		auto y_to_X_null_a = get("a");
		auto y_to_X_null_b = get("b");
		settings_.endGroup();
		settings_.beginGroup("dx_to_dX");
		auto dx_to_dX_a = get("a");
		auto dx_to_dX_b = get("b");
		settings_.endGroup();
		settings_.endGroup();
		settings_.endGroup();

		for(auto const& pair: {
			y_to_Z_a,
			y_to_Z_b,
			y_to_Z_c,
			y_to_Z_d,
			y_to_X_null_a,
			y_to_X_null_b,
			dx_to_dX_a,
			dx_to_dX_b
		}) if(!pair.second) return;


		using namespace mitrax::literals;

		polynom< double, 3 > y_to_Z(
			mitrax::make_col_vector< double >(4_R, {
				y_to_Z_d.first,
				y_to_Z_c.first,
				y_to_Z_b.first,
				y_to_Z_a.first
			}));

		polynom< double, 1 > y_to_X_null(
			mitrax::make_col_vector< double >(2_R, {
				y_to_X_null_b.first,
				y_to_X_null_a.first
			}));

		polynom< double, 1 > dx_to_dX(
			mitrax::make_col_vector< double >(2_R, {
				dx_to_dX_b.first,
				dx_to_dX_a.first
			}));

		calibration calib;
		calib.set(y_to_Z, y_to_X_null, dx_to_dX);
		measure_w_.set_calibration(calib);


		mcl3_.set_position(0, 0, 0);
	}


}
