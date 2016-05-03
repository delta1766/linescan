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


	/// \brief 1D N-degree polynomial
	template < typename T, std::size_t Degree >
	class polynom{
	public:
		/// \brief Type of the input and output data
		using value_type = T;

		/// \brief Degree of the polynom
		constexpr static std::size_t degree = Degree;


		/// \brief All coefficients 0
		constexpr polynom() = default;

		/// \brief Init with given coefficients
		constexpr polynom(
			mitrax::raw_col_vector< value_type, Degree + 1 > const& coeffs
		): coefficients_(coeffs) {}

		/// \brief Set a coefficient
		constexpr void set(std::size_t i, value_type const& v){
			assert(i < Degree + 1);
			coefficients_[i] = v;
		}

		/// \brief Get a coefficient
		constexpr value_type operator[](std::size_t i)const{
			return i < Degree + 1 ? coefficients_[i] : 0;
		}


		/// \brief Get function value
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


	/// \brief Create a first-degree polynom from two points
	template < typename T >
	constexpr polynom< T, 1 > to_polynom(
		mitrax::point< T > const& p1, mitrax::point< T > const& p2
	){
		using namespace mitrax::literals;

		double dx = p2.x() - p1.x();
		double dy = p2.y() - p1.y();
		if(dx == 0) throw std::runtime_error(
				"Can not make linear function, y coordinates are identical"
			);

		auto m = dy / dx;
		auto a = p1.y() - m * p1.x();
		return polynom< double, 1 >(
			mitrax::make_col_vector< double >(2_R, {a, m})
		);
	}


	/// \brief Fit a polynom to a list of 2D-Points
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


	/// \brief Calc intersection of two first-degree polynoms
	template < typename T >
	constexpr T intersection(
		polynom< T, 1 > const& lhs,
		polynom< T, 1 > const& rhs
	){
		return (lhs[0] - rhs[0]) / (rhs[1] - lhs[1]);
	}

	/// \brief Add two polynoms of same degree
	template < typename T, std::size_t Degree >
	constexpr polynom< T, Degree >& operator+=(
		polynom< T, Degree >& a, polynom< T, Degree > const& b
	){
		for(std::size_t i = 0; i < Degree + 1; ++i){
			a.set(i, a[i] + b[i]);
		}
		return a;
	}

	/// \brief Substract two polynoms of same degree
	template < typename T, std::size_t Degree >
	constexpr polynom< T, Degree >& operator-=(
		polynom< T, Degree >& a, polynom< T, Degree > const& b
	){
		for(std::size_t i = 0; i < Degree + 1; ++i){
			a.set(i, a[i] - b[i]);
		}
		return a;
	}

	/// \brief Add two polynoms of same degree
	template < typename T, std::size_t Degree >
	constexpr polynom< T, Degree > operator+(
		polynom< T, Degree > const& a, polynom< T, Degree > const& b
	){
		auto r = a;
		return r += b;
	}

	/// \brief Substract two polynoms of same degree
	template < typename T, std::size_t Degree >
	constexpr polynom< T, Degree > operator-(
		polynom< T, Degree > const& a, polynom< T, Degree > const& b
	){
		auto r = a;
		return r -= b;
	}


	/// \brief Multiply a polynom with a value
	template < typename T, std::size_t Degree >
	constexpr polynom< T, Degree >& operator*=(
		polynom< T, Degree >& a, T const& b
	){
		for(std::size_t i = 0; i < Degree + 1; ++i){
			a.set(i, a[i] * b);
		}
		return a;
	}

	/// \brief Divide a polynom by a value
	template < typename T, std::size_t Degree >
	constexpr polynom< T, Degree >& operator/=(
		polynom< T, Degree >& a, T const& b
	){
		for(std::size_t i = 0; i < Degree + 1; ++i){
			a.set(i, a[i] / b);
		}
		return a;
	}

	/// \brief Multiply a polynom with a value
	template < typename T, std::size_t Degree >
	constexpr polynom< T, Degree > operator*(
		polynom< T, Degree > const& a, T const& b
	){
		auto r = a;
		return r *= b;
	}

	/// \brief Divide a polynom by a value
	template < typename T, std::size_t Degree >
	constexpr polynom< T, Degree > operator/(
		polynom< T, Degree > const& a, T const& b
	){
		auto r = a;
		return r /= b;
	}


}


#endif
