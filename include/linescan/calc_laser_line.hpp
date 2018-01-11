//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__calc_laser_line__hpp_INCLUDED_
#define _linescan__calc_laser_line__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <QImage>

#include <vector>


namespace linescan{


	/// \brief Contains all laser line calculation types
	namespace calc_laser_line_mode{
		/// \brief The 4 visualizations of threshold line calculation
		enum class threshold{
			original,
			binarize,
			erode,
			line
		};

		/// \brief The 2 visualizations of sum line calculation
		enum class sum{
			original,
			line
		};
	};


	/// \brief Overload type for visualization via calc_laser_line_t
	struct as_image_t{};

	/// \brief Overload object for visualization via calc_laser_line_t
	constexpr auto as_image = as_image_t();


	/// \brief Overload type for visualization and points via calc_laser_line_t
	struct points_and_image_t{};

	/// \brief Overload object for visualization  and points via
	///        calc_laser_line_t
	constexpr auto points_and_image = points_and_image_t();


	/// \brief Transform a camera image into vector of 2D laser line points
	class calc_laser_line_t{
	public:
		/// \brief Calculate only the vector of laser 2D line points
		std::vector< mitrax::point< double > >
		operator()(mitrax::std_bitmap< std::uint8_t > const& image)const;

		/// \brief Calculate only the visualization
		QImage operator()(
			mitrax::std_bitmap< std::uint8_t > const& image,
			as_image_t
		)const;

		/// \brief Calculate the vector of laser 2D line points and the
		///        visualization
		std::pair< std::vector< mitrax::point< double > >, QImage >
		operator()(
			mitrax::std_bitmap< std::uint8_t > const& image,
			points_and_image_t
		)const;

		/// \brief Set threshold as method and its parameters
		void use(
			calc_laser_line_mode::threshold mode,
			std::uint8_t threshold,
			std::size_t erode,
			bool subpixel
		);

		/// \brief Set sum as method and its parameters
		void use(
			calc_laser_line_mode::sum mode,
			std::uint8_t min_value,
			std::size_t min_sum,
			bool subpixel
		);

	private:
		enum class type{
			threshold,
			sum
		};

		type method_ = type::threshold;

		calc_laser_line_mode::threshold threshold_mode_;
		std::uint8_t threshold_ = 255;
		std::size_t erode_ = 2;
		bool threshold_subpixel_ = true;

		calc_laser_line_mode::sum sum_mode_;
		std::uint8_t min_value_ = 20;
		std::size_t min_sum_ = 4000;
		bool sum_subpixel_ = true;
	};

	/// \brief Global instance of calc_laser_line_t
	extern calc_laser_line_t calc_laser_line;


}


#endif
