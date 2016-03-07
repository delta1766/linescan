//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__vector__hpp_INCLUDED_
#define _linescan__vector__hpp_INCLUDED_

#include <boost/container/vector.hpp>


namespace linescan{


	/// \brief Use boost::container::vector as linescan::vector
	template < typename T >
	using vector = boost::container::vector< T >;


}


#endif
