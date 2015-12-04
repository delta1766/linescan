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


	template < typename F, typename ... M, size_t ... C, size_t ... R >
	inline void for_all_pixel(
		F const& f,
		mitrax::matrix< M, C, R > const& ... images
	){
		auto size = mitrax::get_dims(images ...);
		for(std::size_t y = 0; y < size.rows(); ++y){
			for(std::size_t x = 0; x < size.cols(); ++x){
				f(images(x, y) ...);
			}
		}
	}


	namespace detail{


		template < typename F, typename ... M >
		struct make_matrix_pixel_wise_t{
			constexpr make_matrix_pixel_wise_t(
				F const& f, M const& ... images
			): f(f), images(images ...) {}

			F const& f;
			std::tuple< M const& ... > images;

			constexpr auto operator()(size_t x, size_t y)const{
				return (*this)(x, y, std::index_sequence_for< M ... >());
			}

			template < size_t ... I >
			constexpr auto operator()(
				size_t x, size_t y, std::index_sequence< I ... >
			)const{
				return f(std::get< I >(images)(x, y) ...);
			}
		};


	}


	template < typename F, typename ... M, size_t ... C, size_t ... R >
	constexpr auto make_matrix_pixel_wise(
		F const& f,
		mitrax::matrix< M, C, R > const& ... images
	){
		auto size = mitrax::get_dims(images ...);

		return mitrax::make_matrix_by_function(size,
			detail::make_matrix_pixel_wise_t<
				F, mitrax::matrix< M, C, R > ...
			>(f, images ...));
	}


	template < typename F, typename ... T >
	inline auto offset_view_wise(
		F const& f,
		std::size_t view_size_x,
		std::size_t view_size_y,
		mitrax::raw_bitmap< T > const& ... images
	){
		namespace hana = boost::hana;

		auto sizes = mitrax::get_dims(images ...);

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
