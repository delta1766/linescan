//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
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


	std::vector< point< float > > find_chessboard_corners(camera& cam){
		using namespace mitrax::literals;

		auto image = cam.image();

		std::uint8_t* data = image.impl().data().data();

		cv::Mat bitmap(
			cv::Size(image.cols(), image.rows()),
			CV_8U,
			static_cast< void* >(data)
		);

		std::vector< cv::Point2f > points;
		auto found = cv::findChessboardCorners(
			bitmap,
			cv::Size(9, 6),
			points,
			CV_CALIB_CB_ADAPTIVE_THRESH |
			CV_CALIB_CB_FAST_CHECK |
			CV_CALIB_CB_NORMALIZE_IMAGE
		);

		if(!found) std::runtime_error("no chessboard corners found");

		cv::cornerSubPix(
			bitmap, points, cv::Size(11,11), cv::Size(-1,-1),
			cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1)
		);

		std::vector< point< float > > result;
		result.reserve(points.size());
		std::transform(
			points.begin(), points.end(),
			std::back_inserter(result),
			[](cv::Point2f const& p){
				return point< float >(p.x, p.y);
			}
		);

		return result;
	}


}
