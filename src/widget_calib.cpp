//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_calib.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <cmath>
#include <iostream>


namespace linescan{


	namespace{


		QImage draw_overlay(
			mitrax::bitmap_dims_t const& size,
			mitrax::raw_bitmap< circle > const& circles
		){
			QImage overlay(
				QSize(size.cols(), size.rows()),
				QImage::Format_ARGB32
			);
			overlay.fill(0);

			QPainter painter(&overlay);
			painter.setRenderHint(QPainter::Antialiasing, true);

			// draw circles in red
			painter.setPen(QPen(QBrush(Qt::red), 1));
			for(auto const& c: circles){
				painter.drawEllipse(
					QPointF(c.x(), c.y()), c.radius(), c.radius()
				);
			}

			// draw outlines in green
			painter.setPen(QPen(QBrush(Qt::green), 1));
			auto p1 = circles(0, 0);
			auto p2 = circles(0, std::size_t(circles.rows()) - 1);
			auto p3 = circles(std::size_t(circles.cols()) - 1, 0);
			auto p4 = circles(
				std::size_t(circles.cols()) - 1,
				std::size_t(circles.rows()) - 1
			);
			auto P1 = QPointF(p1.x(), p1.y());
			auto P2 = QPointF(p2.x(), p2.y());
			auto P3 = QPointF(p3.x(), p3.y());
			auto P4 = QPointF(p4.x(), p4.y());
			painter.drawLine(P1, P2);
			painter.drawLine(P1, P3);
			painter.drawLine(P2, P4);
			painter.drawLine(P3, P4);

			// draw outline cross in green
			painter.drawLine(P1, P4);
			painter.drawLine(P2, P3);

			// draw grid in yellow
			painter.setPen(QPen(QBrush(Qt::yellow), 1));
			for(std::size_t y = 0; y < circles.rows(); ++y){
				for(std::size_t x = 1; x < circles.cols(); ++x){
					auto p1 = circles(x - 1, y);
					auto p2 = circles(x, y);
					painter.drawLine(
						QPointF(p1.x(), p1.y()), QPointF(p2.x(), p2.y())
					);
				}
			}
			for(std::size_t y = 1; y < circles.rows(); ++y){
				for(std::size_t x = 0; x < circles.cols(); ++x){
					auto p1 = circles(x, y - 1);
					auto p2 = circles(x, y);
					painter.drawLine(
						QPointF(p1.x(), p1.y()), QPointF(p2.x(), p2.y())
					);
				}
			}

			return overlay;
		}

		std::pair< mitrax::raw_bitmap< std::uint8_t >, QImage > draw_circles(
			mitrax::raw_bitmap< std::uint8_t >&& bitmap
		){
			try{
				auto circles = circlefind(bitmap, 12, 9, 1, 2.5);

				auto overlay = draw_overlay(bitmap.dims(), circles);

				return { std::move(bitmap), overlay };
			}catch(std::exception const& error){
				std::cerr << "Exception: " << error.what() << std::endl;
			}catch(...){
				std::cerr << "Unknown exception" << std::endl;
			}
			return { std::move(bitmap), QImage() };
		}


		std::array< float, 3 > calc_intrinsic_parameters(
			mitrax::bitmap_dims_t const& image_size,
			std::vector< mitrax::raw_bitmap< circle > > const& circles_list,
			float distance_in_mm
		){
			std::vector< std::vector< cv::Point3f > > object_points;
			std::vector< std::vector< cv::Point2f > > image_points;
			for(auto const& circles: circles_list){
				std::vector< cv::Point3f > object_tmp;
				std::vector< cv::Point2f > image_tmp;
				for(std::size_t y = 0; y < circles.rows(); ++y){
					for(std::size_t x = 0; x < circles.cols(); ++x){
						object_tmp.emplace_back(
							x * distance_in_mm, y * distance_in_mm, 0
						);
						image_tmp.emplace_back(
							circles(x, y).x(), circles(x, y).y()
						);
					}
				}
				object_points.push_back(std::move(object_tmp));
				image_points.push_back(std::move(image_tmp));
			}

			cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
			camera_matrix.at< float >(0, 0) = 1.0;

			cv::Mat distortion_coefficients = cv::Mat::zeros(4, 1, CV_64F);

			auto flags =
				CV_CALIB_FIX_K1 | CV_CALIB_FIX_K2 | CV_CALIB_FIX_K3 |
				CV_CALIB_FIX_K4 | CV_CALIB_FIX_K5 | CV_CALIB_FIX_K6 |
				CV_CALIB_ZERO_TANGENT_DIST;
			flags |= CV_CALIB_FIX_ASPECT_RATIO;
// 			flags |= CV_CALIB_FIX_PRINCIPAL_POINT;

			// Find intrinsic and extrinsic camera parameters
			std::vector< cv::Mat > rotation_vectors;
			std::vector< cv::Mat > translation_vectors;
			cv::calibrateCamera(
				object_points,
				image_points,
				cv::Size(image_size.cols(), image_size.rows()),
				camera_matrix,
				distortion_coefficients,
				rotation_vectors,
				translation_vectors,
				flags
			);

			if(
				!cv::checkRange(camera_matrix) ||
				!cv::checkRange(distortion_coefficients)
			){
				throw std::runtime_error("cv::calibrateCamera() failed");
			}

			return {{
				camera_matrix.at< float >(0, 0),
				camera_matrix.at< float >(0, 2),
				camera_matrix.at< float >(1, 2)
			}};
		}


	}


	widget_calib::widget_calib(camera& cam):
		image_(cam, [this](bool is_live){
			if(is_live){
				intrinsic_button_.setText(tr("Capture"));
			}else{
				intrinsic_button_.setText(tr("Live"));
			}
		}),
		intrinsic_image_count_(tr("0 Images")),
		intrinsic_auto_(tr("Auto Capture")),
		intrinsic_focal_length_(tr("Focal length")),
		intrinsic_principal_point_(tr("Principal point"))
	{
		std::size_t icon_rows = 100;
		std::size_t icon_cols = static_cast< std::size_t >(
			std::ceil(float(icon_rows) * cam.cols() / cam.rows()));

		intrinsic_images_.setIconSize(QSize(icon_cols, icon_rows));
		tabs_.setMinimumWidth(icon_cols + 50);
		tabs_.setMaximumWidth(icon_cols + 50);

		intrinsic_layout_.addWidget(&intrinsic_image_count_);
		intrinsic_layout_.addWidget(&intrinsic_images_, 1);
		intrinsic_layout_.addWidget(&intrinsic_button_);
		intrinsic_layout_.addWidget(&intrinsic_auto_);
		intrinsic_layout_.addWidget(&intrinsic_focal_length_);
		intrinsic_layout_.addWidget(&intrinsic_principal_point_);
		intrinsic_auto_.setCheckable(true);

		intrinsics_.setLayout(&intrinsic_layout_);

		tabs_.addItem(&intrinsics_, "Intrinsics");
		tabs_.addItem(&extrinsics_, "Extrinsics");
		tabs_.addItem(&movements_, "Movements");

		layout_.addWidget(&tabs_);
		layout_.addWidget(&image_, 1);

		image_.set_processor(&draw_circles);

		connect(&intrinsic_button_, &QPushButton::released, [this, icon_rows]{
			if(image_.is_live()){
				image_.stop_live();

				auto bitmap = image_.bitmap();

				try{
					auto distance_mm = 2.5f;
					auto radius_mm = 1.f;

					auto circles = circlefind(
						bitmap, 12, 9, radius_mm, distance_mm
					);

					circles = fine_fit(bitmap, circles, radius_mm, distance_mm);

					auto image = image_.image().convertToFormat(
						QImage::Format_RGB32
					).scaledToHeight(icon_rows);

					auto factor = image.width() / float(bitmap.cols());
					auto mini_circles = circles;
					for(auto& c: mini_circles){
						c.x() *= factor;
						c.y() *= factor;
						c.radius() *= factor;
					}

					auto overlay = draw_overlay(
						mitrax::dims(image.width(), image.height()),
						mini_circles 
					);

					{
						QPainter painter(&image);
						auto x = float(image.width() - overlay.width()) / 2;
						auto y = float(image.height() - overlay.height()) / 2;

						painter.drawImage(x, y, overlay);
					}

					auto icon = QIcon(QPixmap::fromImage(image));

					image_.set_overlay(draw_overlay(bitmap.dims(), circles));
					new QListWidgetItem(icon, "", &intrinsic_images_);
					intrinsic_images_.scrollToBottom();

					circles_list_.push_back(std::move(circles));

					auto parameters = calc_intrinsic_parameters(
						bitmap.dims(),
						circles_list_,
						distance_mm
					);

					intrinsic_focal_length_.setText(
						tr("Focal length:\n%1 mm").arg(parameters[0])
					);

					intrinsic_principal_point_.setText(
						tr("Principal point:\n%1x%2 px")
							.arg(parameters[1], 0, 'f', 0)
							.arg(parameters[2], 0, 'f', 0)
					);

					intrinsic_image_count_.setText(
						tr("%1 Images").arg(circles_list_.size())
					);
				}catch(...){
					QMessageBox box(
						QMessageBox::Warning,
						tr("Error"),
						tr("Circles not found."),
						QMessageBox::Ok
					);

					box.exec();

					image_.start_live();
				}
			}else{
				image_.start_live();
			}
		});

		setLayout(&layout_);
	}


}
