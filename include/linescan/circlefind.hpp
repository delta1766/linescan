//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__circlefind__hpp_INCLUDED_
#define _linescan__circlefind__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <vector>


namespace linescan{


	class circle{
	public:
		constexpr circle()noexcept:
			center_(0, 0),
			radius_(0)
			{}

		constexpr circle(float x, float y, float radius)noexcept:
			center_(x, y),
			radius_(radius)
			{}

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


		constexpr mitrax::point< float >& center()noexcept{
			return center_;
		}

		constexpr mitrax::point< float > center()const noexcept{
			return center_;
		}

		constexpr float& x()noexcept{ return center_.x(); }

		constexpr float x()const noexcept{ return center_.x(); }

		constexpr float& y()noexcept{ return center_.y(); }

		constexpr float y()const noexcept{ return center_.y(); }

		constexpr float& radius()noexcept{ return radius_; }

		constexpr float radius()const noexcept{ return radius_; }

		constexpr float diameter()const noexcept{ return radius() * 2; }


	private:
		mitrax::point< float > center_;
		float radius_;
	};


	circle fit_circle(
		mitrax::raw_bitmap< float > const& image,
		float x_from, float x_length, std::size_t x_steps,
		float y_from, float y_length, std::size_t y_steps,
		float r_from, float r_length, std::size_t r_steps
	);

	mitrax::raw_bitmap< circle > circlefind(
		mitrax::raw_bitmap< std::uint8_t > const& image,
		std::size_t x_count, std::size_t y_count,
		float radius_in_mm, float distance_in_mm
	);

	mitrax::raw_bitmap< circle > finefit(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap,
		mitrax::raw_bitmap< circle > circles,
		float radius_mm, float distance_mm
	);


}


#endif
