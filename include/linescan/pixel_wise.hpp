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

#include <mitrax/matrix.hpp>

#include <boost/hana.hpp>


namespace linescan{


	namespace detail{ namespace pixel_wise{


		template < typename T >
		auto size(mitrax::raw_bitmap< T > const& image){
			return image.dims();
		}

		template < typename T, typename U, typename ... R >
		auto size(
			mitrax::raw_bitmap< T > const& image1,
			mitrax::raw_bitmap< U > const& image2,
			mitrax::raw_bitmap< R > const& ... images
		){
			if(image1.dims() != image2.dims()){
				throw std::logic_error(
					"pixel_wise have been called with images with different "
					"sizes"
				);
			}

			return size(image2, images ...);
		}


	} }


	template < typename F, typename ... T >
	inline auto pixel_wise(F const& f, mitrax::raw_bitmap< T > const& ... images){
		namespace hana = boost::hana;

		auto size = detail::pixel_wise::size(images ...);

		return hana::if_(
			hana::traits::is_void(
				hana::type_c< decltype(f(std::declval< T >() ...)) >
			),
			[&size, &images ...](auto& f){
				for(std::size_t y = 0; y < size.rows(); ++y){
					for(std::size_t x = 0; x < size.cols(); ++x){
						f(images(x, y) ...);
					}
				}
			},
			[&size, &images ...](auto& f){
				auto result = mitrax::make_matrix<
					decltype(f(std::declval< T >() ...)) >(size);

				for(std::size_t y = 0; y < size.rows(); ++y){
					for(std::size_t x = 0; x < size.cols(); ++x){
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
		mitrax::raw_bitmap< T > const& ... images
	){
		namespace hana = boost::hana;

		auto sizes = detail::pixel_wise::size(images ...);

		auto const vsx = view_size_x;
		auto const vsy = view_size_y;

		return hana::if_(
			hana::traits::is_void(hana::type_c< decltype(
				f(std::declval< mitrax::raw_bitmap< T > >() ...)
			) > ),
			[vsx, vsy, &sizes, &images ...](auto& f){
				for(std::size_t y = 0; y < sizes.rows() - vsy; ++y){
					for(std::size_t x = 0; x < sizes.cols() - vsx; ++x){
						f(images.sub_matrix(x, y, mitrax::dims(vsx, vsy)) ...);
					}
				}
			},
			[vsx, vsy, &sizes, &images ...](auto& f){
				auto result = mitrax::make_matrix< decltype(
					f(images.sub_matrix(0, 0, mitrax::dims(vsx, vsy)) ...)
				) >(mitrax::dims(sizes.cols() - vsx, sizes.rows() - vsy));

				for(std::size_t y = 0; y < sizes.rows() - vsy; ++y){
					for(std::size_t x = 0; x < sizes.cols() - vsx; ++x){
						result(x, y) = f(images.sub_matrix(
							x, y, mitrax::dims(vsx, vsy)
						) ...);
					}
				}

				return result;
			}
		)(f);
	}


}


#endif
