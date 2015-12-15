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
#include <linescan/load.hpp>
#include <linescan/draw.hpp>
#include <linescan/binarize.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/calc_top_distance_line.hpp>
#include <linescan/erode.hpp>

#include <mitrax/io_debug.hpp>
#include <mitrax/point_io.hpp>

#include <boost/type_index.hpp>

#include <iostream>


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
		dock_(tr("Control"), this),
		laser_align_("Laser &algin", this),
		calib_intrinsic_("Calibrate &Intrinsic Parameters", this),
		calib_extrinsic_("Calibrate &Extrinsic Parameters", this),
		intrinsic_get_("&Get", this),
		intrinsic_ready_("&Ready", this),
		laser_ok_("&OK", this)
	{
		view_.setScene(&scene_);
		item_.setPixmap(QPixmap("data/start.jpg"));
		scene_.addItem(&item_);
		setCentralWidget(&view_);

		calib_extrinsic_.setEnabled(false);

		main_dock_layout_.addWidget(&laser_align_);
		main_dock_layout_.addWidget(&calib_intrinsic_);
		main_dock_layout_.addWidget(&calib_extrinsic_);
		main_dock_widget_.setLayout(&main_dock_layout_);

		intrinsic_dock_layout_.addWidget(&intrinsic_get_);
		intrinsic_dock_layout_.addWidget(&intrinsic_ready_);
		intrinsic_dock_widget_.setLayout(&intrinsic_dock_layout_);

		extrinsic_dock_layout_.addWidget(&extrinsic_get_);
		extrinsic_dock_widget_.setLayout(&extrinsic_dock_layout_);

		laser_dock_layout_.addWidget(&laser_ok_);
		laser_dock_widget_.setLayout(&laser_dock_layout_);

		main_dock_widget_.show();
		intrinsic_dock_widget_.hide();
		laser_dock_widget_.hide();

		dock_layout_.setContentsMargins(0, 0, 0, 0);
		dock_layout_.addWidget(&laser_dock_widget_);
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
		laser_timer_.setInterval(250);

		connect(&laser_align_, &QPushButton::clicked, [this]{
			main_dock_widget_.hide();
			laser_dock_widget_.show();
			extrinsic_dock_widget_.hide();
			intrinsic_dock_widget_.hide();

			cam_.set_default_light();

			laser_timer_.start();
		});

		connect(&laser_ok_, &QPushButton::clicked, [this]{
			main_dock_widget_.show();
			laser_dock_widget_.hide();
			extrinsic_dock_widget_.hide();
			intrinsic_dock_widget_.hide();

			laser_timer_.stop();
		});

		connect(&calib_intrinsic_, &QPushButton::clicked, [this]{
			main_dock_widget_.hide();
			laser_dock_widget_.hide();
			extrinsic_dock_widget_.hide();
			intrinsic_dock_widget_.show();

			cam_.set_max_light();

			points_.clear();

			timer_.start();
		});

		connect(&intrinsic_get_, &QPushButton::clicked, [this]{
			using mitrax::operator<<;

			timer_.stop();

			exception_catcher([this]{
				auto points = find_chessboard_corners(cam_);

				std::cout << points << std::endl;

				if(!points.empty()) points_.push_back(std::move(points));
			});

			timer_.start();
		});

		connect(&intrinsic_ready_, &QPushButton::clicked, [this]{
			timer_.stop();

			exception_catcher([this]{
				intrinsic_parameters_ = calc_intrinsic_parameters(cam_, points_);
				calib_extrinsic_.setEnabled(true);
			});

			main_dock_widget_.show();
			laser_dock_widget_.hide();
			extrinsic_dock_widget_.hide();
			intrinsic_dock_widget_.hide();
		});

		connect(&timer_, &QTimer::timeout, [this]{
			show_bitmap(cam_.image());
		});

		connect(&laser_timer_, &QTimer::timeout, [this]{
			try{
#ifdef CAM
				auto bitmap = cam_.image();
#else
				auto bitmap = load("simulation/real2_laser.png");
#endif

				auto binary = binarize(bitmap, std::uint8_t(255));
				binary = erode(binary, 3);

				auto top_distance_line = calc_top_distance_line(binary);

				std::vector< point< double > > points;
				for(std::size_t i = 0; i < top_distance_line.size(); ++i){
					if(top_distance_line[i] == 0) continue;
					points.emplace_back(i, top_distance_line[i]);
				}

				if(points.size() < 2){
					show_bitmap(bitmap);
					return;
				}

				auto line = fit_linear_function< double >(
					points.begin(), points.end()
				);

				QImage image(
					bitmap.impl().data().data(),
					bitmap.cols(), bitmap.rows(),
					QImage::Format_Grayscale8
				);

				auto pixmap = QPixmap::fromImage(image);

				{
					QPainter painter(&pixmap);
					painter.setPen(qRgb(255, 0, 0));
					painter.drawLine(
						0, line(0), pixmap.width() - 1, line(pixmap.width() - 1)
					);
				}

				item_.setPixmap(pixmap);
			}catch(std::exception const& e){
				std::cerr
					<< "Exit with exception: ["
					<< boost::typeindex::type_id_runtime(e).pretty_name() << "] "
					<< e.what() << std::endl;
			}catch(...){
				std::cerr << "Exit with unknown exception" << std::endl;
			}
		});
	}

	main_window::~main_window(){
		scene_.removeItem(&item_);
	}

	void main_window::show_bitmap(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap
	){
		QImage image(
			bitmap.impl().data().data(),
			bitmap.cols(), bitmap.rows(),
			QImage::Format_Grayscale8
		);

		item_.setPixmap(QPixmap::fromImage(image));
	}


}
