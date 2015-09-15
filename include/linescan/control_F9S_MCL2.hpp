//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan_control_F9S_MCL2_hpp_INCLUDED_
#define _linescan_control_F9S_MCL2_hpp_INCLUDED_

#include "control_F9S_base.hpp"

#include <iostream>


namespace linescan{


	class control_F9S_MCL2: public control_F9S_base{
	public:
		using control_F9S_base::control_F9S_base;


		void read_status(){
			send({{read::status}});
		}

		void start_calibration(){
			send({{write::command, 'c'}, {read::start}});
		}

		void stop(){
			send({{write::command, 'a'}, {read::start}});
		}

		void start(){
			send({{read::start}});
		}


	private:
		struct read{
			enum: std::uint8_t{
				preselection_x = 64,
				preselection_y = 65,
				absolute_position_x = 67,
				absolute_position_y = 68,
				status = 70,
				command = 71,
				ramp = 72,
				motor_speed = 73,
				current_reduction = 74,
				mask = 75,
				delay_time_for_replies = 76,
				start = 80,
				activate_cts = 81
			};
		};

		struct write{
			enum: std::uint8_t{
				preselection_x = 0,
				preselection_y = 1,
				absolute_position_x = 3,
				absolute_position_y = 4,
				status = 6,
				command = 7,
				ramp = 8,
				motor_speed = 9,
				current_reduction = 10,
				mask = 11,
				delay_time_for_replies = 12,
				activate_cts = 17
			};
		};
	};


}


#endif
