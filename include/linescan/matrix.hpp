//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__matrix__hpp_INCLUDED_
#define _linescan__matrix__hpp_INCLUDED_

#include <utility>
#include <array>


namespace linescan{


	template < typename ValueType, std::size_t Cols, std::size_t Rows >
	class matrix{
	public:
		/// \brief Type of the data that administrates the bitmap
		using value_type = ValueType;

		/// \brief Type of points in the bitmap
		using point_type = point< std::size_t >;

		/// \brief Type of bitmap size
		using size_type = linescan::size< std::size_t >;

		/// \brief Type of a iterator for data
		using iterator =
			std::iterator< std::random_access_iterator_tag, value_type >;

		/// \brief Type of a iterator for const data
		using const_iterator =
			std::iterator< std::random_access_iterator_tag, value_type const >;

		/// \brief Type of a reverse iterator for data
		using reverse_iterator = std::reverse_iterator< iterator >;

		/// \brief Type of a reverse iterator for const data
		using const_reverse_iterator = std::reverse_iterator< iterator const >;

		/// \brief Type of a reference to data
		using reference = value_type&;

		/// \brief Type of a reference to const data
		using const_reference = value_type const&;

		/// \brief Type of a pointer to data
		using pointer = value_type*;

		/// \brief Type of a pointer to const data
		using const_pointer = value_type const*;


		constexpr matrix():
			values_{{0}} {}


		constexpr matrix(value_type const(&values)[Rows][Cols]):
			values_(to_array(
				values,
				std::make_index_sequence< Cols * Rows >()
			)){}

		constexpr matrix(value_type(&&values)[Rows][Cols]):
			values_(to_array(
				values,
				std::make_index_sequence< Cols * Rows >()
			)){}

// 		template < typename ... T >
// 		constexpr matrix(T const& ... v):
// 			values_{ static_cast< value_type >(v) ... } {}

		constexpr matrix(matrix&&) = default;

		constexpr matrix(matrix const&) = default;


		constexpr value_type& operator()(
			std::size_t x, std::size_t y
		){
			return values_[y * Cols + x];
		}

		constexpr value_type const& operator()(
			std::size_t x, std::size_t y
		)const{
			return values_[y * Cols + x];
		}


		static constexpr std::size_t width(){
			return Cols;
		}

		static constexpr std::size_t height(){
			return Rows;
		}


	private:
		std::array< value_type, Cols * Rows > values_;

		template < std::size_t ... I >
		static constexpr auto to_array(
			value_type const(&values)[Rows][Cols],
			std::index_sequence< I ... >
		){
			return std::array< value_type, Cols * Rows >{{
				values[I / Rows][I % Rows] ...
			}};
		}

		template < std::size_t ... I >
		static constexpr auto to_array(
			value_type(&&values)[Rows][Cols],
			std::index_sequence< I ... >
		){
			return std::array< value_type, Cols * Rows >{{
				std::move(values[I / Rows][I % Rows]) ...
			}};
		}
	};


	template < typename T, std::size_t N >
	using square_matrix = matrix< T, N, N >;

	template < typename T, std::size_t Rows >
	using row_vector = matrix< T, 1, Rows >;

	template < typename T, std::size_t Cols >
	using col_vector = matrix< T, Cols, 1 >;


}

#endif
