//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/control_F9S_MCL3.hpp>


namespace linescan{


	control_F9S_MCL3::control_F9S_MCL3(std::string const& device):
		control_F9S_base(device)
	{
		std::cout << "MCL3" << std::endl;
		for(std::size_t i = 64; i < 90; ++i){
			if(i == 77) continue;
			if(i == 78) continue;
			if(i == 79) continue;
			if(i == 80) continue;
			if(i == 81) continue;
			if(i == 82) continue;
			if(i == 83) continue;
			if(i == 84) continue;
			if(i == 88) continue;
			std::cout << "get " << i << std::endl;
			send({i});
			receive();
		}

// 		write_resolution(10);
// 		write_ramp(10);
// 		write_leadscrew_pitch_x(10000);
// 		write_leadscrew_pitch_y(10000);
// 		write_leadscrew_pitch_z(10000);
// 
// 		if(read_resolution() != 10) throw std::logic_error("init resolution error");
// 		if(read_ramp() != 10) throw std::logic_error("init ramp error");
// 		if(read_leadscrew_pitch_x() != 10000) throw std::logic_error("init leadscrew_pitch_x error");
// 		if(read_leadscrew_pitch_y() != 10000) throw std::logic_error("init leadscrew_pitch_y error");
// 		if(read_leadscrew_pitch_z() != 10000) throw std::logic_error("init leadscrew_pitch_z error");
	}


	void control_F9S_MCL3::calibrate(){
		send({{write::command, 'c'}, {read::start}});

		// TODO: respect mask
		auto answer = receive();
		if(answer == "AAA-.") return;

		throw std::logic_error(
			"answer after calibrate was '" + answer +
			"', expected was 'AAA-.'"
		);
	}

	void control_F9S_MCL3::move_to_end(){
		send({{write::command, 'l'}, {read::start}});

		// TODO: respect mask
		auto answer = receive();
		if(answer == "DDD-.") return;

		throw std::logic_error(
			"answer after move to end was '" + answer +
			"', expected was 'DDD-.'"
		);
	}

	void control_F9S_MCL3::stop(){
		send({{write::command, 'a'}, {read::start}});

		auto answer = receive();
		if(regex_search(answer, move_answer_expected)) return;

		throw std::logic_error(
			"answer after stop was '" + answer + "'"
		);
	}

	void control_F9S_MCL3::set_position(std::int64_t x, std::int64_t y, std::int64_t z){
		send({
			{write::absolute_position_x, x},
			{write::absolute_position_y, y},
			{write::absolute_position_z, z}
		});
		delay();
	}

	void control_F9S_MCL3::move_to(std::int64_t x, std::int64_t y, std::int64_t z){
		send({
			{write::preselection_x, x},
			{write::preselection_y, y},
			{write::preselection_z, z},
			{write::command, 'r'},
			{read::start}
		});

		auto answer = receive();
		if(regex_search(answer, move_answer_expected)) return;

		throw std::logic_error(
			"answer after move_to was '" + answer + "'"
		);
	}

	void control_F9S_MCL3::move_relative(std::int64_t x, std::int64_t y, std::int64_t z){
		send({
			{write::preselection_x, x},
			{write::preselection_y, y},
			{write::preselection_z, z},
			{write::command, 'v'},
			{read::start}
		});

		auto answer = receive();
		if(regex_search(answer, move_answer_expected)) return;

		throw std::logic_error(
			"answer after move_relative was '" + answer + "'"
		);
	}

	std::array< std::int64_t, 3 > control_F9S_MCL3::position(){
		return {{
			read_x(),
			read_y(),
			read_z()
		}};
	}

	std::array< std::int64_t, 3 > control_F9S_MCL3::preselection(){
		return {{
			read_pre_x(),
			read_pre_y(),
			read_pre_z()
		}};
	}


	std::regex const control_F9S_MCL3::move_answer_expected("^[AD@]{3}\\-\\.$");


	std::int64_t control_F9S_MCL3::read_pre_x(){
		send({{read::preselection_x}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_pre_y(){
		send({{read::preselection_y}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_pre_z(){
		send({{read::preselection_z}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_x(){
		send({{read::absolute_position_x}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_y(){
		send({{read::absolute_position_y}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_z(){
		send({{read::absolute_position_z}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	// TODO: Return a struct with parsed data, throw in error case
	std::string control_F9S_MCL3::read_status(){
		send({{read::status}});
		return receive();
	}

	// TODO: Make command an enum
	char control_F9S_MCL3::read_command(){
		send({{read::command}});
		return receive().at(0);
	}

	std::int64_t control_F9S_MCL3::read_ramp(){
		send({{read::ramp}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_motor_speed(){
		send({{read::motor_speed}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_current_reduction(){
		send({{read::current_reduction}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	// TODO: Return a struct with parsed data, throw in error case
	std::string control_F9S_MCL3::read_mask(){
		send({{read::mask}});
		return receive();
	}

	std::int64_t control_F9S_MCL3::read_reply_delay(){
		send({{read::delay_time_for_replies}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_leadscrew_pitch_x(){
		send({{read::leadscrew_pitch_x}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_leadscrew_pitch_y(){
		send({{read::leadscrew_pitch_y}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_leadscrew_pitch_z(){
		send({{read::leadscrew_pitch_z}});
		return boost::lexical_cast< std::int64_t >(receive());
	}

	std::int64_t control_F9S_MCL3::read_resolution(){
		send({{read::resolution}});
		return boost::lexical_cast< std::int64_t >(receive());
	}


	void control_F9S_MCL3::write_leadscrew_pitch_x(std::int64_t value){
		send({{write::leadscrew_pitch_x, value}});
		delay();
	}

	void control_F9S_MCL3::write_leadscrew_pitch_y(std::int64_t value){
		send({{write::leadscrew_pitch_y, value}});
		delay();
	}

	void control_F9S_MCL3::write_leadscrew_pitch_z(std::int64_t value){
		send({{write::leadscrew_pitch_z, value}});
		delay();
	}

	void control_F9S_MCL3::write_ramp(std::int64_t value){
		send({{write::ramp, value}});
		delay();
	}

	void control_F9S_MCL3::write_motor_speed(std::int64_t value){
		send({{write::motor_speed, value}});
		delay();
	}

	void control_F9S_MCL3::write_resolution(std::int64_t value){
		send({{write::resolution, value}});
		delay();
	}


}
