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
			polynom< double, 1 > const& y_to_X_null,
			polynom< double, 1 > const& dx_to_dX
		){
			y_to_Z_ = y_to_Z;
			y_to_X_null_ = y_to_X_null;
			dx_to_dX_ = dx_to_dX;
			valid_ = true;
		}

		double Z(double y)const{
			return y_to_Z_(y);
		}

		double X(double x, double y)const{
			return dx_to_dX_(x - y_to_X_null_(y));
		}

		bool is_valid()const{ return valid_;}

		polynom< double, 3 > y_to_Z()const{ return y_to_Z_;}
		polynom< double, 1 > y_to_X_null()const{ return y_to_X_null_;}
		polynom< double, 1 > dx_to_dX()const{ return dx_to_dX_;}

	private:
		bool valid_ = false;
		polynom< double, 3 > y_to_Z_;
		polynom< double, 1 > y_to_X_null_;
		polynom< double, 1 > dx_to_dX_;
	};


}


#endif
