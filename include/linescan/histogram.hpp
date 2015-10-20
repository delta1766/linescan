//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__save_histogram__hpp_INCLUDED_
#define _linescan__save_histogram__hpp_INCLUDED_

#include "bitmap.hpp"


namespace linescan{


	bitmap< std::uint8_t > save_histogram(bitmap< std::uint8_t > const& image);


}


#endif
