//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__median__hpp_INCLUDED_
#define _linescan__median__hpp_INCLUDED_

#include <mitrax/transform.hpp>


namespace linescan{


	/// \brief median filter
	///
	/// Filter has no border handling, so it does shrink the image.
	template < typename T, size_t C, size_t R, size_t Ck, size_t Rk >
	constexpr auto median(
		mitrax::matrix< T, C, R > const& image,
		mitrax::auto_dim_pair_t< Ck, Rk > const& view_dims
	){
		return mitrax::transform_per_view(
			[](auto&& m){
				std::sort(m.begin(), m.end());
				return m.begin()[
					std::size_t(m.cols()) * std::size_t(m.rows()) / 2
				];
			},
			view_dims,
			image
		);
	}


}


#endif
