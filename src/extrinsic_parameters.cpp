//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/extrinsic_parameters.hpp>

#include <linescan/camera.hpp>
#include <linescan/binarize.hpp>
#include <linescan/erode.hpp>
#include <linescan/invert.hpp>
#include <linescan/collect_points.hpp>
#include <linescan/load.hpp>

#include <mitrax/point_io.hpp>
#include <mitrax/matrix.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>
#include <iomanip>


namespace linescan{


	namespace ref3d{


		using namespace mitrax;
		using namespace mitrax::literals;


		constexpr auto plain_x_p0 = make_col_vector< double >(3_R, {  0, 160,  20});
		constexpr auto plain_x_p1 = make_col_vector< double >(3_R, {  0, 160, 160});
		constexpr auto plain_x_p2 = make_col_vector< double >(3_R, {  0,  20,  20});
		constexpr auto plain_x_p3 = make_col_vector< double >(3_R, {  0,  20, 160});
		constexpr auto plain_y_p0 = make_col_vector< double >(3_R, { 20,   0,  20});
		constexpr auto plain_y_p1 = make_col_vector< double >(3_R, { 20,   0, 160});
		constexpr auto plain_y_p2 = make_col_vector< double >(3_R, {160,   0,  20});
		constexpr auto plain_y_p3 = make_col_vector< double >(3_R, {160,   0, 160});

		constexpr std::array< raw_col_vector< double, 2 >, 4 > ref_x{{
			make_col_vector< double >(2_R, {plain_x_p2[1], plain_x_p2[2]}),
			make_col_vector< double >(2_R, {plain_x_p3[1], plain_x_p3[2]}),
			make_col_vector< double >(2_R, {plain_x_p0[1], plain_x_p0[2]}),
			make_col_vector< double >(2_R, {plain_x_p1[1], plain_x_p1[2]})
		}};

		constexpr std::array< raw_col_vector< double, 2 >, 4 > ref_y{{
			make_col_vector< double >(2_R, {plain_y_p2[0], plain_y_p2[2]}),
			make_col_vector< double >(2_R, {plain_y_p3[0], plain_y_p3[2]}),
			make_col_vector< double >(2_R, {plain_y_p0[0], plain_y_p0[2]}),
			make_col_vector< double >(2_R, {plain_y_p1[0], plain_y_p1[2]})
		}};


	}


	template < typename M, size_t C >
	auto reduce(mitrax::col_vector< M, C > const& v){
		using namespace mitrax::literals;

		if(v[v.rows() - 1_R] == 0) throw std::logic_error(
			"can not reduce col_vector dimension, last coordinate is 0"
		);

		return mitrax::make_col_vector_by_function(
			v.rows() - 1_R, [&v](size_t i){
				return v[i] / v[v.rows() - 1_R];
			});
	}

	template < typename M, size_t C >
	auto expand(mitrax::col_vector< M, C > const& v){
		using namespace mitrax::literals;

		return mitrax::make_col_vector_by_function(
			v.rows() + 1_R, [&v](size_t i){
				return i == v.rows() ? 1 : v[i];
			});
	}


	std::array< point< double >, 8 > find_ref_points(camera& cam){
#ifdef CAM
		auto image = cam.image();
#else
		(void)cam;
		auto image = load("simulation/real2_ref.png");
#endif

		auto binary = binarize(image, std::uint8_t(50));
		binary = erode(binary, 3, true);
		binary = invert(binary);

		auto point_and_counts = collect_points(binary);

		if(point_and_counts.size() < 8){
			throw std::logic_error(
				"not 8 points fount in reference image"
			);
		}else if(point_and_counts.size() > 8){
			// Sort by size
			std::sort(point_and_counts.begin(), point_and_counts.end(),
				[](auto& a, auto& b){
					return a.second > b.second;
				});

			for(auto& v: point_and_counts){
				std::cout << std::setw(8) << v.second << ": " << v.first
					<< std::endl;
			}

			// keep the 8 biggest, remove the rest
			point_and_counts.erase(
				point_and_counts.begin() + 8,
				point_and_counts.end()
			);
		}

		std::array< point< double >, 8 > ref_points{{
			point_and_counts[0].first,
			point_and_counts[1].first,
			point_and_counts[2].first,
			point_and_counts[3].first,
			point_and_counts[4].first,
			point_and_counts[5].first,
			point_and_counts[6].first,
			point_and_counts[7].first,
		}};

		std::sort(ref_points.begin(), ref_points.end(),
			[](auto const& a, auto const& b){
				return a.x() < b.x();
			});

		auto pred_y = [](auto const& a, auto const& b){
				return a.y() < b.y();
			};

		std::sort(ref_points.begin() + 0, ref_points.begin() + 2, pred_y);
		std::sort(ref_points.begin() + 2, ref_points.begin() + 4, pred_y);
		std::sort(ref_points.begin() + 4, ref_points.begin() + 6, pred_y);
		std::sort(ref_points.begin() + 6, ref_points.begin() + 8, pred_y);

		return ref_points;
	}


	std::array< mitrax::raw_col_vector< double, 3 >, 2 >
	calc_extrinsic_parameters(
		std::array< double, 3 > const& camera_matrix_parameter,
		std::array< point< double >, 8 > const& points
	){
		using namespace ref3d;

		std::vector< cv::Point2f > image_points;
		for(auto const& p: points) image_points.emplace_back(p.x(), p.y());

		std::vector< cv::Point3f > object_points{
			cv::Point3f(plain_x_p0[0], plain_x_p0[1], plain_x_p0[2]),
			cv::Point3f(plain_x_p1[0], plain_x_p1[1], plain_x_p1[2]),
			cv::Point3f(plain_x_p2[0], plain_x_p2[1], plain_x_p2[2]),
			cv::Point3f(plain_x_p3[0], plain_x_p3[1], plain_x_p3[2]),
			cv::Point3f(plain_y_p0[0], plain_y_p0[1], plain_y_p0[2]),
			cv::Point3f(plain_y_p1[0], plain_y_p1[1], plain_y_p1[2]),
			cv::Point3f(plain_y_p2[0], plain_y_p2[1], plain_y_p2[2]),
			cv::Point3f(plain_y_p3[0], plain_y_p3[1], plain_y_p3[2])
		};

		cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
		camera_matrix.at< double >(0, 0) = camera_matrix_parameter[0];
		camera_matrix.at< double >(1, 1) = camera_matrix_parameter[0];
		camera_matrix.at< double >(2, 0) = camera_matrix_parameter[1];
		camera_matrix.at< double >(2, 1) = camera_matrix_parameter[2];

		// TODO: Use the real ones
		cv::Mat distortion_coefficients = cv::Mat::zeros(8, 1, CV_64F);

		cv::Mat rotation_vector;
		cv::Mat translation_vector;
		cv::solvePnP(
			object_points,
			image_points,
			camera_matrix,
			distortion_coefficients,
			rotation_vector,
			translation_vector
		);

		return {{
			mitrax::make_col_vector< double >(3_R, {
				rotation_vector.at< double >(0, 0),
				rotation_vector.at< double >(1, 0),
				rotation_vector.at< double >(2, 0)
			}),
			mitrax::make_col_vector< double >(3_R, {
				translation_vector.at< double >(0, 0),
				translation_vector.at< double >(1, 0),
				translation_vector.at< double >(2, 0)
			})
		}};
	}


}
