//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__to_array__hpp_INCLUDED_
#define _linescan__to_array__hpp_INCLUDED_

#include <array>
#include <type_traits>
#include <utility>


namespace linescan{


	namespace detail{


		template < typename T, std::size_t N, std::size_t ... I >
		constexpr auto to_array(T(&arr)[N], std::index_sequence< I ... >){
			return std::array< std::remove_cv_t< T >, N >{{ arr[I] ... }};
		}


	}


	template < typename T, std::size_t N >
	constexpr auto to_array(T(&arr)[N]){
		return detail::to_array(arr, std::make_index_sequence< N >());
	}


}


#endif
