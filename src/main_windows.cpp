//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/main_window.hpp>

#include <linescan/intrinsic_parameters.hpp>

#include <mitrax/io_debug.hpp>
#include <mitrax/point_io.hpp>

#include <iostream>


namespace linescan{


	main_window::main_window():
		mcl3_("/dev/ttyUSB0"),
		cam_(0),
		dock_(tr("Control"), this),
		calib_intrinsic_("Calibrate &Intrinsic Parameters", this),
		calib_extrinsic_("Calibrate &Extrinsic Parameters", this),
		intrinsic_get_("&Get", this),
		intrinsic_ready_("&Ready", this)
	{
		view_.setScene(&scene_);
		item_.setPixmap(QPixmap("data/start.jpg"));
		scene_.addItem(&item_);
		setCentralWidget(&view_);

		main_dock_layout_.addWidget(&calib_intrinsic_);
		main_dock_layout_.addWidget(&calib_extrinsic_);
		main_dock_widget_.setLayout(&main_dock_layout_);

		intrinsic_dock_layout_.addWidget(&intrinsic_get_);
		intrinsic_dock_layout_.addWidget(&intrinsic_ready_);
		intrinsic_dock_widget_.setLayout(&intrinsic_dock_layout_);

		main_dock_widget_.show();
		intrinsic_dock_widget_.hide();

		dock_layout_.setContentsMargins(0, 0, 0, 0);
		dock_layout_.addWidget(&main_dock_widget_);
		dock_layout_.addWidget(&intrinsic_dock_widget_);
		dock_widget_.setLayout(&dock_layout_);
		dock_.setWidget(&dock_widget_);

		dock_.setAllowedAreas(
			Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea
		);
		dock_.setFeatures(QDockWidget::DockWidgetMovable);
		addDockWidget(Qt::TopDockWidgetArea, &dock_);

		timer_.setInterval(1000);

		connect(
			&calib_intrinsic_, &QPushButton::clicked,
			this,  &main_window::intrinsic_calibrate
		);

		connect(
			&intrinsic_get_, &QPushButton::clicked,
			this,  &main_window::capture_intrinsic_image
		);

		connect(
			&intrinsic_ready_, &QPushButton::clicked,
			this,  &main_window::ready_intrinsic_calibrate
		);

		connect(
			&timer_, &QTimer::timeout,
			this,  &main_window::live
		);
	}


	main_window::~main_window(){
		scene_.removeItem(&item_);
	}


	void main_window::set_image(QPixmap const& pixmap){
		item_.setPixmap(pixmap);
	}


	void main_window::intrinsic_calibrate(){
		main_dock_widget_.hide();
		intrinsic_dock_widget_.show();

		auto set_max_light = [this](){
			auto framerate = cam_.framerate_max();
			cam_.set_framerate(framerate);
			auto pixelclock = cam_.pixelclock_min();
			cam_.set_pixelclock(pixelclock);
			auto exposure = cam_.exposure_in_ms_max();
			cam_.set_exposure(exposure);
			cam_.set_gain(100);
			cam_.set_gain_boost(true);
			cam_.image();
		};

		set_max_light();

		points_.clear();

		timer_.start();
	}


	void main_window::capture_intrinsic_image(){
		using mitrax::operator<<;

		timer_.stop();

		try{
			auto points = find_chessboard_corners(cam_);

			std::cout << points << std::endl;

			if(!points.empty()) points_.push_back(std::move(points));
		}catch(std::exception const& error){
			QMessageBox box(
				QMessageBox::Warning,
				tr("Error"),
				error.what(),
				QMessageBox::Ok
			);

			box.exec();
		}

		timer_.start();
	}


	void main_window::ready_intrinsic_calibrate(){
		
	}


	void main_window::live(){
		auto bitmap = cam_.image();

		QImage image(
			bitmap.impl().data().data(),
			bitmap.cols(), bitmap.rows(),
			QImage::Format_Grayscale8
		);

		set_image(QPixmap::fromImage(image));
	}


}
