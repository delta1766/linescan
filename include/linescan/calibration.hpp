//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__calibration__hpp_INCLUDED_
#define _linescan__calibration__hpp_INCLUDED_

#include "polynom.hpp"


namespace linescan{


	class calibration{
	public:
		void set(
			polynom< double, 3 > const& y_to_Z,
			polynom< double, 1 > const& y_to_Y_null,
			polynom< double, 1 > const& dx_to_dY
		){
			y_to_Z_ = y_to_Z;
			y_to_Y_null_ = y_to_Y_null;
			dx_to_dY_ = dx_to_dY;
			valid_ = true;
		}

		double Z(double y)const{
			return y_to_Z_(y);
		}

		double Y(double x, double y)const{
			return dx_to_dY_(x - y_to_Y_null_(y));
		}

		bool is_valid()const{ return valid_;}

		polynom< double, 3 > y_to_Z()const{ return y_to_Z_;}
		polynom< double, 1 > y_to_Y_null()const{ return y_to_Y_null_;}
		polynom< double, 1 > dx_to_dY()const{ return dx_to_dY_;}

	private:
		bool valid_ = false;
		polynom< double, 3 > y_to_Z_;
		polynom< double, 1 > y_to_Y_null_;
		polynom< double, 1 > dx_to_dY_;
	};


}


#endif
