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


	class laser_calibration{
	public:
		void set(
			polynom< double, 3 > const& cam_y_to_z_map,
			polynom< double, 1 > const& left_limit,
			polynom< double, 1 > const& right_limit
		){
			cam_y_to_z_map_ = cam_y_to_z_map;
			left_limit_ = left_limit;
			right_limit_ = right_limit;
			valid_ = false;
		}

		double z(double cam_y)const{
			return cam_y_to_z_map_(cam_y);
		}

		double left_limit(double cam_y)const{
			return left_limit_(cam_y);
		}

		double right_limit(double cam_y)const{
			return right_limit_(cam_y);
		}

	private:
		bool valid_ = false;
		polynom< double, 3 > cam_y_to_z_map_;
		polynom< double, 1 > left_limit_;
		polynom< double, 1 > right_limit_;
	};


}


#endif
