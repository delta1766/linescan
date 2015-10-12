//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__control_F9S_MCL3__hpp_INCLUDED_
#define _linescan__control_F9S_MCL3__hpp_INCLUDED_

#include "control_F9S_base.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <thread>
#include <regex>


namespace linescan{


	class control_F9S_MCL3: public control_F9S_base{
	public:
		control_F9S_MCL3(std::string const& device):
			control_F9S_base(device)
		{
			write_resolution(10);
			write_ramp(10);
			write_leadscrew_pitch_x(10000);
			write_leadscrew_pitch_y(10000);
			write_leadscrew_pitch_z(10000);

			read_x();
			read_y();
			read_z();
			read_pre_x();
			read_pre_y();
			read_pre_z();
			read_resolution();
			read_ramp();
			read_leadscrew_pitch_x();
			read_leadscrew_pitch_y();
			read_leadscrew_pitch_z();
		}


		/// \brief Move to start and null absolute position registers
		void calibrate(){
			send({{write::command, 'c'}, {read::start}});

			auto answer = receive();
			if(answer == "AAA-.") return;

			throw std::logic_error(
				"answer after calibrate was '" + answer +
				"', expected was 'AAA-.'"
			);
		}

		/// \brief Move to end position
		///
		/// After a calibrate you can move to end and read ranges
		/// from the absolut position registers.
		void move_to_end(){
			send({{write::command, 'l'}, {read::start}});

			auto answer = receive();
			if(answer == "DDD-.") return;

			throw std::logic_error(
				"answer after move to end was '" + answer +
				"', expected was 'DDD-.'"
			);
		}

		/// \brief Stop all movements
		void stop(){
			send({{write::command, 'a'}, {read::start}});

			static std::regex expected("^[AD@]{3}\\-\\.$");
			auto answer = receive();
			if(regex_search(answer, expected)) return;

			throw std::logic_error(
				"answer after stop was '" + answer + "'"
			);
		}

		void move_to(std::int64_t x, std::int64_t y, std::int64_t z){
			write_pre_x(x);
			write_pre_y(y);
			write_pre_z(z);
			send({{write::command, 'e'}, {read::start}});
		}

		void move_relative(std::int64_t x, std::int64_t y, std::int64_t z){
			write_pre_x(x);
			write_pre_y(y);
			write_pre_z(z);
			send({{write::command, 'g'}, {read::start}});
		}


	private:
		/// \brief sleep for 50ms
		///
		/// After write commands without answer you should wait a few ms.
		void delay()const{
			using namespace std::literals;
			std::this_thread::sleep_for(50ms);
		}

		std::int64_t read_pre_x(){
			send({{read::preselection_x}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_pre_y(){
			send({{read::preselection_y}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_pre_z(){
			send({{read::preselection_z}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_x(){
			send({{read::absolute_position_x}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_y(){
			send({{read::absolute_position_y}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_z(){
			send({{read::absolute_position_z}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		// TODO: Return a struct with parsed data, throw in error case
		std::string read_status(){
			send({{read::status}});
			return receive();
		}

		// TODO: Make command an enum
		char read_command(){
			send({{read::command}});
			return receive().at(0);
		}

		std::int64_t read_ramp(){
			send({{read::ramp}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_motor_speed(){
			send({{read::motor_speed}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_current_reduction(){
			send({{read::current_reduction}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		// TODO: Return a struct with parsed data, throw in error case
		std::string read_mask(){
			send({{read::mask}});
			return receive();
		}

		std::int64_t read_reply_delay(){
			send({{read::delay_time_for_replies}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_leadscrew_pitch_x(){
			send({{read::leadscrew_pitch_x}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_leadscrew_pitch_y(){
			send({{read::leadscrew_pitch_y}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_leadscrew_pitch_z(){
			send({{read::leadscrew_pitch_z}});
			return boost::lexical_cast< std::int64_t >(receive());
		}

		std::int64_t read_resolution(){
			send({{read::resolution}});
			return boost::lexical_cast< std::int64_t >(receive());
		}


		void write_pre_x(std::int64_t value){
			send({{write::preselection_x, value}});
			delay();
		}

		void write_pre_y(std::int64_t value){
			send({{write::preselection_y, value}});
			delay();
		}

		void write_pre_z(std::int64_t value){
			send({{write::preselection_z, value}});
			delay();
		}

		void write_x(std::int64_t value){
			send({{write::absolute_position_x, value}});
			delay();
		}

		void write_y(std::int64_t value){
			send({{write::absolute_position_y, value}});
			delay();
		}

		void write_z(std::int64_t value){
			send({{write::absolute_position_z, value}});
			delay();
		}

		void write_leadscrew_pitch_x(std::int64_t value){
			send({{write::leadscrew_pitch_x, value}});
			delay();
		}

		void write_leadscrew_pitch_y(std::int64_t value){
			send({{write::leadscrew_pitch_y, value}});
			delay();
		}

		void write_leadscrew_pitch_z(std::int64_t value){
			send({{write::leadscrew_pitch_z, value}});
			delay();
		}

		void write_ramp(std::int64_t value){
			send({{write::ramp, value}});
			delay();
		}

		void write_motor_speed(std::int64_t value){
			send({{write::motor_speed, value}});
			delay();
		}

		void write_resolution(std::int64_t value){
			send({{write::resolution, value}});
			delay();
		}


		struct read{
			enum: std::uint8_t{
				preselection_x = 64,
				preselection_y = 65,
				preselection_z = 66,
				absolute_position_x = 67,
				absolute_position_y = 68,
				absolute_position_z = 69,
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
				leadscrew_pitch_z = 87,
				resolution = 89
			};
		};

		struct write{
			enum: std::uint8_t{
				preselection_x = 0,
				preselection_y = 1,
				preselection_z = 2,
				absolute_position_x = 3,
				absolute_position_y = 4,
				absolute_position_z = 5,
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
				leadscrew_pitch_z = 23,
				resolution = 25
			};
		};
	};


}


#endif
