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

#include <mitrax/convolution.hpp>

#include <cmath>


namespace linescan{


	template < std::size_t Size, typename T >
	mitrax::raw_bitmap< std::uint8_t > gauss(
		mitrax::raw_bitmap< std::uint8_t > const& image,
		T const& variance
	){
		auto vc = mitrax::make_col_vector< T >(mitrax::row_t< Size >().init());
		auto vr = mitrax::make_row_vector< T >(mitrax::col_t< Size >().init());

		T sum = 0;
		for(std::size_t i = 0; i < Size; ++i){
			auto x = i - Size / T(2) + T(.5);

			auto value =
				std::exp(- ((x * x) / (2 * variance * variance))) /
				(variance * std::sqrt(2 * M_PI));

			vc[i] = value;

			sum += value;
		}

		for(std::size_t i = 0; i < Size; ++i){
			vc[i] /= sum;
			vr[i] = vc[i];
		}

		return transform([](auto v){
			return static_cast< std::uint8_t >(v);
		}, convolution(image, vc, vr));
	}


}


#endif
