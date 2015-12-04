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


namespace linescan{


	namespace detail{


		template < typename F, typename S, typename ... T >
		struct invoke_t{
			constexpr invoke_t(
				F const& f, S const& single_call, T const& ... arg
			): f_(f), single_call_(single_call), arg_(arg ...) {}

			F const& f_;
			S const& single_call_;
			std::tuple< T const& ... > arg_;

			template < typename ... A >
			constexpr auto operator()(A&& ... arg)const{
				return (*this)(std::index_sequence_for< T ... >(), arg ...);
			}

			template < size_t ... I, typename ... A >
			constexpr auto operator()(
				std::index_sequence< I ... >, A&& ... arg
			)const{
				return f_(single_call_(std::get< I >(arg_), arg ...) ...);
			}
		};

		template < typename F, typename S, typename ... T >
		constexpr auto
		make_invoke(F const& f, S const& single_call, T const& ... arg){
			return invoke_t< F, S, T ... >(f, single_call, arg ...);
		}

		struct call_func_operator{
			template < typename M >
			constexpr auto operator()(M const& m, size_t x, size_t y)const{
				return m(x, y);
			}
		};

		template < bool Ccto, size_t Co, bool Rcto, size_t Ro >
		struct call_sub_matrix{
			mitrax::col_init_t< Ccto, Co > cols;
			mitrax::row_init_t< Rcto, Ro > rows;

			template < typename M >
			constexpr auto operator()(M const& m, size_t x, size_t y)const{
				return m.sub_matrix(x, y, cols, rows);
			}
		};

		template < bool Ccto, size_t Co, bool Rcto, size_t Ro >
		constexpr auto make_call_sub_matrix(
			mitrax::col_init_t< Ccto, Co > cols,
			mitrax::row_init_t< Rcto, Ro > rows
		){
			return call_sub_matrix< Ccto, Co, Rcto, Ro >{cols, rows};
		}


	}


	template < typename F, typename ... M, size_t ... C, size_t ... R >
	constexpr void for_all_pixel(
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


	template <
		typename F, bool Ccto, size_t Co, bool Rcto, size_t Ro, 
		typename ... M, size_t ... C, size_t ... R
	> constexpr void for_all_view(
		F const& f,
		mitrax::col_init_t< Ccto, Co > view_cols,
		mitrax::row_init_t< Rcto, Ro > view_rows,
		mitrax::matrix< M, C, R > const& ... images
	){
		auto cols = mitrax::get_cols(images ...);
		auto rows = mitrax::get_rows(images ...);
		for(std::size_t y = 0; y < rows - view_rows; ++y){
			for(std::size_t x = 0; x < cols - view_cols; ++x){
				f(images.sub_matrix(x, y, view_cols, view_rows) ...);
			}
		}
	}


	template < typename F, typename ... M, size_t ... C, size_t ... R >
	constexpr auto make_matrix_pixel_wise(
		F const& f,
		mitrax::matrix< M, C, R > const& ... images
	){
		return mitrax::make_matrix_by_function(mitrax::get_dims(images ...),
			detail::make_invoke(f, detail::call_func_operator(), images ...));
	}


	template <
		typename F, bool Ccto, size_t Co, bool Rcto, size_t Ro, 
		typename ... M, size_t ... C, size_t ... R
	> constexpr auto make_matrix_view_wise(
		F const& f,
		mitrax::col_init_t< Ccto, Co > view_cols,
		mitrax::row_init_t< Rcto, Ro > view_rows,
		mitrax::matrix< M, C, R > const& ... images
	){
		using namespace mitrax::literals;
		return mitrax::make_matrix_by_function(
			mitrax::get_cols(images ...) - view_cols + 1_C,
			mitrax::get_rows(images ...) - view_rows + 1_R,
			detail::make_invoke(
				f, detail::make_call_sub_matrix(view_cols, view_rows),
				images ...
			));
	}


}


#endif
