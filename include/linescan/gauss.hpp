//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__gauss__hpp_INCLUDED_
#define _linescan__gauss__hpp_INCLUDED_

#include "convolution.hpp"

#include <cmath>


namespace linescan{


	template < std::size_t Size, typename T >
	bitmap< std::uint8_t > gauss(
		bitmap< std::uint8_t > const& image,
		T const& variance
	){
		matrix< T, 1, Size > m1;
		matrix< T, Size, 1 > m2;

		T sum = 0;
		for(std::size_t i = 0; i < Size; ++i){
			auto x = i - Size / T(2) + T(.5);

			auto value =
				std::exp(- ((x * x) / (2 * variance * variance))) /
				(variance * std::sqrt(2 * M_PI));

			m1(0, i) = value;

			sum += value;
		}

		for(std::size_t i = 0; i < Size; ++i){
			m1(0, i) /= sum;
			m2(i, 0) = m1(0, i);
		}

		return pixel_wise([](auto v){
			return static_cast< std::uint8_t >(v);
		}, convolution< T >(convolution< T >(image, m1), m2));
	}


}


#endif
