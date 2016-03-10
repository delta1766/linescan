//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/intrinsic_parameters.hpp>

#include <linescan/camera.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <QtWidgets/QWidget>


namespace linescan{


	auto chessboard_inner_points = mitrax::dims(9, 6);
	auto square_size = 2.5f;

	std::vector< mitrax::point< float > > find_chessboard_corners(
		mitrax::raw_bitmap< std::uint8_t > const& image
	){
		using namespace mitrax::literals;

		auto data = image.impl().data().data();

		cv::Mat bitmap(
			cv::Size(image.cols(), image.rows()),
			CV_8U,
			static_cast< void* >(const_cast< std::uint8_t* >(data))
		);

		std::vector< cv::Point2f > points;
		auto found = cv::findChessboardCorners(
			bitmap,
			cv::Size(
				chessboard_inner_points.cols(),
				chessboard_inner_points.rows()
			),
			points,
			CV_CALIB_CB_ADAPTIVE_THRESH |
			CV_CALIB_CB_FAST_CHECK |
			CV_CALIB_CB_NORMALIZE_IMAGE
		);

		if(!found) std::runtime_error("no chessboard corners found");

		auto point_count =
			static_cast< std::size_t >(chessboard_inner_points.cols()) *
			static_cast< std::size_t >(chessboard_inner_points.rows());

		if(points.size() != point_count){
			throw std::runtime_error("did not find all chessboard corners");
		}

		cv::cornerSubPix(
			bitmap, points, cv::Size(11, 11), cv::Size(-1, -1),
			cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1)
		);

		std::vector< mitrax::point< float > > result;
		result.reserve(points.size());
		std::transform(
			points.begin(), points.end(),
			std::back_inserter(result),
			[](cv::Point2f const& p){
				return mitrax::point< float >(p.x, p.y);
			}
		);

		return result;
	}


	std::tuple< std::array< double, 3 >, std::array< double, 8 > >
	calc_intrinsic_parameters(
		mitrax::bitmap_dims_t const& size,
		std::vector< std::vector< mitrax::point< float > > > const& ref_points
	){
		std::vector< std::vector< cv::Point2f > > image_points;
		for(auto const& ip: ref_points){
			std::vector< cv::Point2f > tmp;
			for(auto const& v: ip){
				tmp.emplace_back(v.x(), v.y());
			}
			image_points.push_back(tmp);
		}

		cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
		camera_matrix.at< double >(0, 0) = 1.0;

		cv::Mat distortion_coefficients = cv::Mat::zeros(8, 1, CV_64F);

		std::vector< std::vector< cv::Point3f > > object_points(1);

		for(std::size_t y = 0; y < chessboard_inner_points.rows(); ++y){
			for(std::size_t x = 0; x < chessboard_inner_points.cols(); ++x){
				object_points[0].push_back(
					cv::Point3f(x * square_size, y * square_size, 0)
				);
			}
		}

		object_points.resize(image_points.size(), std::move(object_points[0]));

		auto flags = CV_CALIB_FIX_K4 | CV_CALIB_FIX_K5;
		flags |= CV_CALIB_FIX_ASPECT_RATIO;
// 		flags |= CV_CALIB_FIX_PRINCIPAL_POINT;

		// Find intrinsic and extrinsic camera parameters
		std::vector< cv::Mat > rotation_vectors;
		std::vector< cv::Mat > translation_vectors;
		cv::calibrateCamera(
			object_points,
			image_points,
			cv::Size(size.cols(), size.rows()),
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

		return std::make_tuple(
			std::array< double, 3 >{{
				camera_matrix.at< double >(0, 0),
				camera_matrix.at< double >(0, 2),
				camera_matrix.at< double >(1, 2)
			}},
			std::array< double, 8 >{{
				distortion_coefficients.at< double >(0, 0),
				distortion_coefficients.at< double >(1, 0),
				distortion_coefficients.at< double >(2, 0),
				distortion_coefficients.at< double >(3, 0),
				distortion_coefficients.at< double >(4, 0),
				distortion_coefficients.at< double >(5, 0),
				distortion_coefficients.at< double >(6, 0),
				distortion_coefficients.at< double >(7, 0)
			}}
		);
	}




}
