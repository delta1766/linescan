//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__composed_function__hpp_INCLUDED_
#define _linescan__composed_function__hpp_INCLUDED_


namespace linescan{


	template < typename T, typename F1, typename F2 >
	struct composed_function{
		using value_type = T;

		constexpr composed_function(
			F1 const& f1,
			value_type const& next,
			F2 const& f2
		):
			f1(f1),
			f2(f2),
			next(next)
			{}

		F1 const f1;
		F2 const f2;
		value_type const next;

		constexpr value_type operator()(value_type const& x)const{
			return x < next ? f1(x) : f2(x);
		}
	};

	template < typename T, typename F1, typename F2 >
	constexpr composed_function< T, F1, F2 > make_composed_function(
			F1 const& f1,
			T const& next,
			F2 const& f2
	){
		return composed_function< T, F1, F2 >(f1, next, f2);
	}


}


#endif
