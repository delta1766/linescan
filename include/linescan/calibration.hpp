//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 Benjamin Buch
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


	/// \brief A valid or invalid calibration
	class calibration{
	public:
		/// \brief Set a valid calibration
		void set(
			polynom< double, 3 > const& y_to_Z,
			polynom< double, 1 > const& y_to_x_null,
			polynom< double, 1 > const& y_to_dx_div_dX
		){
			y_to_Z_ = y_to_Z;
			y_to_x_null_ = y_to_x_null;
			y_to_dx_div_dX_ = y_to_dx_div_dX;
			valid_ = true;
		}

		/// \brief Transform 2D-y-coordinate into 3D-Z-coordinate
		double Z(double y)const{
			return y_to_Z_(y);
		}

		/// \brief Transform 2D-x- and y-coordinate into 3D-X-coordinate
		double X(double x, double y)const{
			return (x - y_to_x_null_(y)) / y_to_dx_div_dX_(y);
		}

		/// \brief true if calibration is valid
		bool is_valid()const{ return valid_;}

		/// \brief Get the 2D-y to 3D-Z polynom
		polynom< double, 3 > y_to_Z()const{ return y_to_Z_;}

		/// \brief Get the 2D-y to 2D-x-null-position polynom
		polynom< double, 1 > y_to_x_null()const{ return y_to_x_null_;}

		/// \brief Get the 2D-y to (difference 2D-x / difference 3D-X) factor
		polynom< double, 1 > y_to_dx_div_dX()const{ return y_to_dx_div_dX_;}

	private:
		bool valid_ = false;
		polynom< double, 3 > y_to_Z_;
		polynom< double, 1 > y_to_x_null_;
		polynom< double, 1 > y_to_dx_div_dX_;
	};


}


#endif
