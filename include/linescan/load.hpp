//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__load__hpp_INCLUDED_
#define _linescan__load__hpp_INCLUDED_

#include <mitrax/matrix.hpp>


namespace linescan{


	/// \brief load an 8-bit png image from hard disk
	mitrax::std_bitmap< std::uint8_t > load(std::string const& name);


}


#endif
