//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__pixel_wise__hpp_INCLUDED_
#define _linescan__pixel_wise__hpp_INCLUDED_

#include "bitmap.hpp"


namespace linescan{


	namespace detail{ namespace pixel_wise{


		template < typename T >
		auto size(bitmap< T > const& image){
			return image.size();
		}

		template < typename T, typename U, typename ... R >
		auto size(
			bitmap< T > const& image1,
			bitmap< U > const& image2,
			bitmap< R > const& ... images
		){
			if(image1.size() != image2.size()){
				throw std::logic_error(
					"pixel_wise have been called with images with different "
					"sizes"
				);
			}

			return size(image2, images ...);
		}


	} }


	template < typename F, typename ... T >
	inline auto pixel_wise(F const& f, bitmap< T > const& ... images){
		bitmap< decltype(f(std::declval< T >() ...)) > result(
			detail::pixel_wise::size(images ...)
		);

		for(std::size_t y = 0; y < result.height(); ++y){
			for(std::size_t x = 0; x < result.width(); ++x){
				result(x, y) = f(images(x, y) ...);
			}
		}

		return result;
	}


}


#endif
