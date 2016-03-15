//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__ref3d__hpp_INCLUDED_
#define _linescan__ref3d__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <array>


namespace linescan{


	namespace ref3d{


		using namespace mitrax;
		using namespace mitrax::literals;


		constexpr auto plain_x_p0 =
			make_col_vector< double >(3_R, {  0, 160,  20});
		constexpr auto plain_x_p1 =
			make_col_vector< double >(3_R, {  0, 160, 160});
		constexpr auto plain_x_p2 =
			make_col_vector< double >(3_R, {  0,  20,  20});
		constexpr auto plain_x_p3 =
			make_col_vector< double >(3_R, {  0,  20, 160});
		constexpr auto plain_y_p0 =
			make_col_vector< double >(3_R, { 20,   0,  20});
		constexpr auto plain_y_p1 =
			make_col_vector< double >(3_R, { 20,   0, 160});
		constexpr auto plain_y_p2 =
			make_col_vector< double >(3_R, {160,   0,  20});
		constexpr auto plain_y_p3 =
			make_col_vector< double >(3_R, {160,   0, 160});

		constexpr std::array< raw_col_vector< double, 2 >, 4 > ref_x{{
			make_col_vector< double >(2_R, {plain_x_p2[1], plain_x_p2[2]}),
			make_col_vector< double >(2_R, {plain_x_p3[1], plain_x_p3[2]}),
			make_col_vector< double >(2_R, {plain_x_p0[1], plain_x_p0[2]}),
			make_col_vector< double >(2_R, {plain_x_p1[1], plain_x_p1[2]})
		}};

		constexpr std::array< raw_col_vector< double, 2 >, 4 > ref_y{{
			make_col_vector< double >(2_R, {plain_y_p2[0], plain_y_p2[2]}),
			make_col_vector< double >(2_R, {plain_y_p3[0], plain_y_p3[2]}),
			make_col_vector< double >(2_R, {plain_y_p0[0], plain_y_p0[2]}),
			make_col_vector< double >(2_R, {plain_y_p1[0], plain_y_p1[2]})
		}};


	}


}


#endif
