//-----------------------------------------------------------------------------
// Copyright (c) 2009-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__polynom__hpp_INCLUDED_
#define _linescan__polynom__hpp_INCLUDED_

#include <mitrax/gaussian_elimination.hpp>

#include <cmath>


namespace linescan{


	template < typename T, std::size_t Degree >
	class polynom{
	public:
		using value_type = T;

		constexpr static std::size_t degree = Degree;


		constexpr polynom() = default;

		constexpr polynom(
			mitrax::raw_col_vector< value_type, Degree + 1 > const& coeffs
		): coefficients_(coeffs) {}


		constexpr value_type& operator[](std::size_t i){
			return coefficients_[i];
		}

		constexpr value_type const& operator[](std::size_t i)const{
			return coefficients_[i];
		}


		constexpr value_type operator()(value_type const& value)const{
			value_type sum = 0;

			for(std::size_t i = 0; i < Degree + 1; ++i){
				value_type mul = 1;
				for(std::size_t j = 0; j < i; ++j) mul *= value;
				sum += mul * coefficients_[i];
			}

			return sum;
		}


	private:
		mitrax::raw_col_vector< T, Degree + 1 > coefficients_;
	};


	template < std::size_t Degree, typename Container >
	auto fit_polynom(Container const& data){
		using value_type = typename Container::value_type::y_value_type;

		if(data.size() <= Degree){
			throw std::logic_error("to less points for polynom fit");
		}

		auto matrix = mitrax::make_square_matrix_by_function(
			mitrax::dims< Degree + 1 >(),
			[&data](std::size_t x, std::size_t y){
				auto sum = value_type();

				for(auto const& value: data){
					sum += std::pow(value.x(), x) * std::pow(value.x(), y);
				}

				return sum;
			}
		);

		auto vector = mitrax::make_col_vector_by_function(
			mitrax::rows< Degree + 1 >(),
			[&data](std::size_t i){
				auto sum = value_type();

				for(auto const& value: data){
					sum += std::pow(value.x(), i) * value.y();
				}

				return sum;
			}
		);

		return polynom< value_type, Degree >(
			mitrax::gaussian_elimination(matrix, vector)
		);
	}


	template < typename T >
	constexpr T intersection(
		polynom< T, 1 > const& lhs,
		polynom< T, 1 > const& rhs
	){
		return (lhs[0] - rhs[0]) / (rhs[1] - lhs[1]);
	}

}


#endif
