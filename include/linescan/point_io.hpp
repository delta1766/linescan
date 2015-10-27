//-----------------------------------------------------------------------------
// Copyright (c) 2009-2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__point_io__hpp_INCLUDED_
#define _linescan__point_io__hpp_INCLUDED_

#include "io.hpp"
#include "point.hpp"

#include <iostream>


namespace linescan{


	template < typename charT, typename traits, typename T >
	std::basic_ostream< charT, traits >& operator<<(
		std::basic_ostream< charT, traits >& os,
		point< T > const& data
	){
		return os << data.x() << "x" << data.y();
	}


	template < typename charT, typename traits, typename T >
	std::basic_istream< charT, traits >& operator>>(
		std::basic_istream< charT, traits >& is,
		point< T >& data
	){
		point< T > tmp;
		is >> tmp.x();
		if(!io::equal(is, 'x')) return is;
		is >> tmp.y();

		data = std::move(tmp);

		return is;
	}


}


#endif
