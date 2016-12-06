//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__circle__hpp_INCLUDED_
#define _linescan__circle__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <vector>
#include <array>


namespace linescan{


	/// \brief A circle consisting of a mid point and a radius
	class circle{
	public:
		/// \brief Construct with all 0
		constexpr circle()noexcept:
			center_(0, 0),
			radius_(0)
			{}

		/// \brief Construct with init values
		constexpr circle(float x, float y, float radius)noexcept:
			center_(x, y),
			radius_(radius)
			{}

		/// \brief Construct with init values
		constexpr circle(
			mitrax::point< float > const& center,
			float radius
		)noexcept:
			center_(center),
			radius_(radius)
			{}

		constexpr circle(circle&&)noexcept = default;

		constexpr circle(circle const&)noexcept = default;


		constexpr circle& operator=(circle&&)noexcept = default;

		constexpr circle& operator=(circle const&)noexcept = default;


		/// \brief Get center position by reference
		constexpr mitrax::point< float >& center()noexcept{
			return center_;
		}

		/// \brief Get center position
		constexpr mitrax::point< float > center()const noexcept{
			return center_;
		}


		/// \brief Get x center position by reference
		constexpr float& x()noexcept{ return center_.x(); }

		/// \brief Get x center position
		constexpr float x()const noexcept{ return center_.x(); }

		/// \brief Get y center position by reference
		constexpr float& y()noexcept{ return center_.y(); }

		/// \brief Get y center position
		constexpr float y()const noexcept{ return center_.y(); }

		/// \brief Get radius by reference
		constexpr float& radius()noexcept{ return radius_; }

		/// \brief Get radius
		constexpr float radius()const noexcept{ return radius_; }

		/// \brief Get diameter
		constexpr float diameter()const noexcept{ return radius() * 2; }


	private:
		mitrax::point< float > center_;
		float radius_;
	};

	/// \brief Get center position as point
	constexpr mitrax::point< double > to_point(circle const& c){
		return { c.x(), c.y() };
	}


	/// \brief Find calib circles on bottom of the image
	std::vector< circle > find_calib_circles(
		mitrax::std_bitmap< std::uint8_t > const& bitmap
	);

	/// \brief Find calib circles near the given positions
	std::vector< circle > find_calib_circles(
		mitrax::std_bitmap< std::uint8_t > const& bitmap,
		circle const& c1, circle const& c2
	);


}


#endif
