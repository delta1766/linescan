//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__save__hpp_INCLUDED_
#define _linescan__save__hpp_INCLUDED_

#include "draw.hpp"
#include "normelize_to_uint8.hpp"

#include <mitrax/convert.hpp>


namespace linescan{


	void save(
		mitrax::raw_bitmap< std::uint8_t > const& image,
		std::string const& name
	);

	void save(
		mitrax::raw_bitmap< bool > const& image,
		std::string const& name
	);

	inline void save(
		mitrax::raw_bitmap< std::int32_t > const& image,
		std::string const& name
	){
		save(normelize_to_uint8(image), name);
	}

	inline void save(
		mitrax::raw_bitmap< float > const& image,
		std::string const& name
	){
		save(normelize_to_uint8(image), name);
	}

	inline void save(
		mitrax::raw_bitmap< std::int8_t > const& image,
		std::string const& name
	){
		save(mitrax::convert< std::uint8_t >(image), name);
	}


}


#endif
