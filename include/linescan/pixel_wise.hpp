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

#include <boost/hana.hpp>


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
		namespace hana = boost::hana;

		auto size = detail::pixel_wise::size(images ...);

		return hana::if_(
			hana::traits::is_void(
				hana::type_c< decltype(f(std::declval< T >() ...)) >
			),
			[&size, &images ...](auto& f){
				for(std::size_t y = 0; y < size.height(); ++y){
					for(std::size_t x = 0; x < size.width(); ++x){
						f(images(x, y) ...);
					}
				}
			},
			[&size, &images ...](auto& f){
				bitmap< decltype(f(std::declval< T >() ...)) > result(size);

				for(std::size_t y = 0; y < size.height(); ++y){
					for(std::size_t x = 0; x < size.width(); ++x){
						result(x, y) = f(images(x, y) ...);
					}
				}

				return result;
			}
		)(f);
	}


	template < typename F, typename ... T >
	inline auto offset_view_wise(
		F const& f,
		std::size_t view_size_x,
		std::size_t view_size_y,
		bitmap< T > const& ... images
	){
		namespace hana = boost::hana;

		auto sizes = detail::pixel_wise::size(images ...);

		auto const vsx = view_size_x;
		auto const vsy = view_size_y;

		return hana::if_(
			hana::traits::is_void(hana::type_c< decltype(
				f(std::declval< const_offset_view< T > >() ...)
			) > ),
			[vsx, vsy, &sizes, &images ...](auto& f){
				for(std::size_t y = 0; y < sizes.height() - vsy; ++y){
					for(std::size_t x = 0; x < sizes.width() - vsx; ++x){
						f(images.offset_view(x, y) ...);
					}
				}
			},
			[vsx, vsy, &sizes, &images ...](auto& f){
				bitmap< decltype(
					f(std::declval< const_offset_view< T > >() ...)
				) > result(
						sizes - size< std::size_t >(vsx, vsy)
					);

				for(std::size_t y = 0; y < sizes.height() - vsy; ++y){
					for(std::size_t x = 0; x < sizes.width() - vsx; ++x){
						result(x, y) = f(images.offset_view(x, y) ...);
					}
				}

				return result;
			}
		)(f);
	}


}


#endif
