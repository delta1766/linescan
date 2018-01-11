//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__save__hpp_INCLUDED_
#define _linescan__save__hpp_INCLUDED_

#include <mitrax/convert.hpp>


namespace linescan{


	/// \brief Save image as PNG
	void save(
		mitrax::std_bitmap< std::uint8_t > const& image,
		std::string const& name
	);

	/// \brief Save image as PNG
	void save(
		mitrax::std_bitmap< bool > const& image,
		std::string const& name
	);


}


#endif
