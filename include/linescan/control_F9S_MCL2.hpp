//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__control_F9S_MCL2__hpp_INCLUDED_
#define _linescan__control_F9S_MCL2__hpp_INCLUDED_

#include "control_F9S_base.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <thread>
#include <regex>


namespace linescan{


	class control_F9S_MCL2: public control_F9S_base{
	public:
		control_F9S_MCL2(std::string const& device);


		/// \brief Stop all movements
		void stop();

		void move_to(std::int64_t x, std::int64_t y);

		void move_relative(std::int64_t x, std::int64_t y);

		void set_position(std::int64_t x, std::int64_t y);

		std::array< std::int64_t, 2 > position();

		std::array< std::int64_t, 2 > preselection();


	private:
		static std::regex const move_answer_expected;

		std::int64_t read_pre_x();

		std::int64_t read_pre_y();

		std::int64_t read_x();

		std::int64_t read_y();

		std::string read_status();

		char read_command();

		std::int64_t read_ramp();

		std::int64_t read_motor_speed();

		std::int64_t read_current_reduction();

		std::string read_mask();

		std::int64_t read_reply_delay();

		std::int64_t read_leadscrew_pitch_x();

		std::int64_t read_leadscrew_pitch_y();

		std::int64_t read_resolution();


		void write_leadscrew_pitch_x(std::int64_t value);

		void write_leadscrew_pitch_y(std::int64_t value);

		void write_ramp(std::int64_t value);

		void write_motor_speed(std::int64_t value);

		void write_resolution(std::int64_t value);


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
				activate_cts = 81,
				leadscrew_pitch_x = 85,
				leadscrew_pitch_y = 86,
				resolution = 89
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
				activate_cts = 17,
				leadscrew_pitch_x = 21,
				leadscrew_pitch_y = 22,
				resolution = 25
			};
		};
	};


}


#endif
