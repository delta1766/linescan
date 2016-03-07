//-----------------------------------------------------------------------------
// Copyright (c) 2009-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__linear_function__hpp_INCLUDED_
#define _linescan__linear_function__hpp_INCLUDED_


namespace linescan{


	template < typename T >
	struct linear_function{
		using value_type = T;

		constexpr linear_function(value_type const& a, value_type const& b):
			a(a),
			b(b)
			{}

		value_type const a;
		value_type const b;

		constexpr value_type operator()(value_type const& x)const{
			return x * a + b;
		}
	};

	template < typename Result, typename Iter >
	linear_function< Result > fit_linear_function(Iter first, Iter last){
		Result sx = 0;
		Result sy = 0;
		Result sxx = 0;
		Result syy = 0;
		Result sxy = 0;
		Result n = 0;

		while (first != last){
			auto x = first->x();
			auto y = first->y();

			sx += static_cast< Result >(x);
			sy += static_cast< Result >(y);
			sxx += static_cast< Result >(x) * static_cast< Result >(x);
			syy += static_cast< Result >(y) * static_cast< Result >(y);
			sxy += static_cast< Result >(x) * static_cast< Result >(y);

			n += 1;

			++first;
		}

		auto const a = (n * sxy - sx * sy) / (n * sxx - sx * sx);
		auto const b = sy / n - a * sx / n;

		return linear_function< Result >(a, b);
	}

	template < typename T >
	constexpr T intersection(
		linear_function< T > const& lhs,
		linear_function< T > const& rhs
	){
		return (lhs.b - rhs.b) / (rhs.a - lhs.a);
	}

}


#endif
