//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/main_window.hpp>

#include <linescan/to_pixmap.hpp>
#include <linescan/align_laser.hpp>
#include <linescan/extrinsic_parameters.hpp>
#include <linescan/intrinsic_parameters.hpp>
#include <linescan/calib_laser.hpp>

#include <mitrax/io_debug.hpp>
#include <mitrax/point_io.hpp>

#include <boost/type_index.hpp>

#include <iostream>
#include <iomanip>


namespace linescan{


	template < typename F >
	void exception_catcher(F&& f)try{
		f();
	}catch(std::exception const& error){
		QMessageBox box(
			QMessageBox::Warning,
			QObject::tr("Error"),
			error.what(),
			QMessageBox::Ok
		);

		box.exec();
	}catch(...){
		QMessageBox box(
			QMessageBox::Critical,
			QObject::tr("Fatal Error"),
			"Unknown exception",
			QMessageBox::Ok
		);

		box.exec();
	}


	main_window::main_window():
		mcl3_("/dev/ttyUSB0"),
		cam_(0),
		dock_(tr("Control panel"), this),
		align_laser_(tr("Laser &align"), this),
		calib_intrinsic_(tr("Calibrate &intrinsic parameters"), this),
		calib_extrinsic_(tr("Calibrate &extrinsic parameters"), this),
		calib_laser_(tr("Calibrate &laser plane"), this),
		intrinsic_get_(tr("&Get image data"), this),
		intrinsic_ready_(tr("&Ready (return to main window)"), this),
		extrinsic_get_(tr("&Get image data"), this),
		extrinsic_ready_(tr("&Ready (return to main window)"), this),
		calib_laser_ok_(tr("&Ready (return to main window)"), this),
		laser_label_(tr("Align"), this),
		laser_ok_(tr("&OK"), this),
		cam_dock_(cam_)
	{
		std::cout << std::fixed << std::setprecision(3);

		setWindowTitle(tr("TU Ilmenau - linescan"));

		view_.setScene(&scene_);
		scene_.addItem(&item_);
		setCentralWidget(&view_);

		calib_extrinsic_.setEnabled(false);
		calib_laser_.setEnabled(false);

		main_dock_layout_.addWidget(&align_laser_);
		main_dock_layout_.addWidget(&calib_intrinsic_);
		main_dock_layout_.addWidget(&calib_extrinsic_);
		main_dock_layout_.addWidget(&calib_laser_);
		main_dock_widget_.setLayout(&main_dock_layout_);

		intrinsic_dock_layout_.addWidget(&intrinsic_label_);
		intrinsic_dock_layout_.addWidget(&intrinsic_get_);
		intrinsic_dock_layout_.addWidget(&intrinsic_ready_);
		intrinsic_dock_widget_.setLayout(&intrinsic_dock_layout_);

		extrinsic_dock_layout_.addWidget(&extrinsic_label_);
		extrinsic_dock_layout_.addWidget(&extrinsic_get_);
		extrinsic_dock_layout_.addWidget(&extrinsic_ready_);
		extrinsic_dock_widget_.setLayout(&extrinsic_dock_layout_);

		laser_dock_layout_.addWidget(&laser_label_);
		laser_dock_layout_.addWidget(&laser_ok_);
		laser_dock_widget_.setLayout(&laser_dock_layout_);

		calib_laser_dock_layout_.addWidget(&calib_laser_label_);
		calib_laser_dock_layout_.addWidget(&calib_laser_ok_);
		calib_laser_dock_widget_.setLayout(&calib_laser_dock_layout_);

		show_dock(dock::main);

		dock_layout_.setContentsMargins(0, 0, 0, 0);
		dock_layout_.addWidget(&laser_dock_widget_);
		dock_layout_.addWidget(&main_dock_widget_);
		dock_layout_.addWidget(&intrinsic_dock_widget_);
		dock_layout_.addWidget(&extrinsic_dock_widget_);
		dock_layout_.addWidget(&calib_laser_dock_widget_);
		dock_widget_.setLayout(&dock_layout_);
		dock_.setWidget(&dock_widget_);

		dock_.setAllowedAreas(
			Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea
		);
		dock_.setFeatures(QDockWidget::DockWidgetMovable);
		addDockWidget(Qt::LeftDockWidgetArea, &dock_);

		addDockWidget(Qt::TopDockWidgetArea, &cam_dock_);

		timer_.setInterval(1000);
		laser_timer_.setInterval(250);

		connect(&align_laser_, &QPushButton::clicked, [this]{
			show_process_image();

			show_dock(dock::align_laser);

			cam_.set_default_light();

			laser_timer_.start();
		});

		connect(&laser_ok_, &QPushButton::clicked, [this]{
			laser_timer_.stop();

			show_dock(dock::main);
		});

		connect(&calib_intrinsic_, &QPushButton::clicked, [this]{
			show_process_image();

			intrinsic_label_.setText(tr("no camera matrix"));

			show_dock(dock::calib_intrinsic);

			cam_.set_max_light();

			timer_.start();
		});

		connect(&intrinsic_get_, &QPushButton::clicked, [this]{
			using mitrax::operator<<;

			timer_.stop();

			show_process_image();

			exception_catcher([this]{
				auto points = find_chessboard_corners(cam_);

				std::cout << points << std::endl;

				if(!points.empty()) points_.push_back(std::move(points));

				std::tie(camera_matrix_parameter_, distortion_coefficients_) =
					calc_intrinsic_parameters(cam_, points_);

				intrinsic_label_.setText(
					QString("f: %1mm, cx: %2px, cy: %3px")
					.arg(camera_matrix_parameter_[0] *
						cam_.pixel_size_in_um() / 1000, 0, 'f', 3)
					.arg(camera_matrix_parameter_[1], 0, 'f', 1)
					.arg(camera_matrix_parameter_[2], 0, 'f', 1)
				);
			});

			timer_.start();
		});

		connect(&intrinsic_ready_, &QPushButton::clicked, [this]{
			timer_.stop();

			if(!points_.empty()){
				points_.clear();
				calib_extrinsic_.setEnabled(true);
			}

			show_dock(dock::main);
		});

		connect(&calib_extrinsic_, &QPushButton::clicked, [this]{
			show_process_image();

			extrinsic_label_.setText(tr("no rotation matrix"));

			show_dock(dock::calib_extrinsic);

			cam_.set_max_light();

			timer_.start();
		});

		connect(&extrinsic_get_, &QPushButton::clicked, [this]{
			timer_.stop();

			show_process_image();

			exception_catcher([this]{
				auto points = find_ref_points(cam_);
				points_3d_.clear();
				points_3d_.insert(
					points_3d_.begin(), points.begin(), points.end()
				);

				std::tie(rotation_vector_, translation_vector_) =
					calc_extrinsic_parameters(
						camera_matrix_parameter_,
						distortion_coefficients_,
						points
					);

				extrinsic_label_.setText(
					QString("rotation(%1, %2, %3), translation(%4, %5, %6)")
					.arg(rotation_vector_[0], 0, 'f', 2)
					.arg(rotation_vector_[1], 0, 'f', 2)
					.arg(rotation_vector_[2], 0, 'f', 2)
					.arg(translation_vector_[0], 0, 'f', 2)
					.arg(translation_vector_[1], 0, 'f', 2)
					.arg(translation_vector_[2], 0, 'f', 2)
				);
			});

			timer_.start();
		});

		connect(&extrinsic_ready_, &QPushButton::clicked, [this]{
			timer_.stop();

			show_process_image();

			if(!points_3d_.empty()){
				calib_laser_.setEnabled(true);
			}

			show_dock(dock::main);
		});

		connect(&calib_laser_, &QPushButton::clicked, [this]{
			show_process_image();

			calib_laser_label_.setText(tr("no plane"));

			show_dock(dock::calib_laser);

			cam_.set_default_light();

			laser_timer_.start();
		});

		connect(&calib_laser_ok_, &QPushButton::clicked, [this]{
			laser_timer_.stop();

			show_process_image();

			show_dock(dock::main);
		});

		connect(&timer_, &QTimer::timeout, [this]{
			auto pixmap = to_pixmap(cam_.image());

			auto draw_points = [&pixmap](auto& points){
				QPainter painter(&pixmap);

				painter.setPen(qRgb(255, 0, 0));
				QFont font = painter.font();
				font.setPixelSize(14);
				painter.setFont(font);

				std::size_t i = 1;
				for(auto const& p: points){
					QRect rect(p.x() - 10, p.y() - 10, 20, 20);
					painter.drawEllipse(rect);
					painter.drawText(
						rect, Qt::AlignCenter, QString("%1").arg(i)
					);
					++i;
				}

			};

			if(!points_.empty()){
				draw_points(points_.back());
			}else{
				draw_points(points_3d_);
			}

			item_.setPixmap(pixmap);
		});

		connect(
			&laser_timer_, &QTimer::timeout,
			this, &main_window::laser_live
		);
	}

	main_window::~main_window(){
		scene_.removeItem(&item_);
	}

	void main_window::laser_live()try{
		if(!laser_dock_widget_.isHidden()){
			QString text;
			QPixmap pixmap;

			std::tie(text, pixmap) = align_laser(cam_);

			laser_label_.setText(text);
			item_.setPixmap(pixmap);
		}else if(!calib_laser_dock_widget_.isHidden()){
			item_.setPixmap(calib_laser_pixmap(cam_, points_3d_));
		}
	}catch(std::exception const& e){
		std::cerr
			<< "Exit with exception: ["
			<< boost::typeindex::type_id_runtime(e).pretty_name()
			<< "] " << e.what() << std::endl;
	}catch(...){
		std::cerr << "Exit with unknown exception" << std::endl;
	}

	void main_window::show_main_image(){
		item_.setPixmap(QPixmap("data/start.jpg"));
		qApp->processEvents();
	}

	void main_window::show_process_image(){
		item_.setPixmap(QPixmap("data/process.png"));
		qApp->processEvents();
	}

	void main_window::show_dock(dock const& d){
		main_dock_widget_.hide();
		laser_dock_widget_.hide();
		extrinsic_dock_widget_.hide();
		intrinsic_dock_widget_.hide();
		calib_laser_dock_widget_.hide();

		switch(d){
			case dock::main:
				main_dock_widget_.show();
				show_main_image();
			break;
			case dock::align_laser: laser_dock_widget_.show(); break;
			case dock::calib_intrinsic: intrinsic_dock_widget_.show(); break;
			case dock::calib_extrinsic: extrinsic_dock_widget_.show(); break;
			case dock::calib_laser: calib_laser_dock_widget_.show(); break;
		}
	}


}
