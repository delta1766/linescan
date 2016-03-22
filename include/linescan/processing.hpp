//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__processing__hpp_INCLUDED_
#define _linescan__processing__hpp_INCLUDED_

#include <mitrax/transform.hpp>


namespace linescan{


	mitrax::raw_bitmap< bool > erode(
		mitrax::raw_bitmap< bool > const& image,
		std::size_t size,
		bool border_value = false
	);

	mitrax::raw_bitmap< std::uint8_t > gauss(
		mitrax::raw_bitmap< std::uint8_t > const& image,
		std::size_t size,
		float variance = 0.7f
	);

	template < typename T >
	inline auto
	binarize(mitrax::raw_bitmap< T > const& image, T const& threshold){
		return mitrax::transform([threshold](auto v){
			return v >= threshold;
		}, image);
	}


}


#endif