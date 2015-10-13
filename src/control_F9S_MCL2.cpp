//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/control_F9S_MCL2.hpp>


namespace linescan{


	control_F9S_MCL2::control_F9S_MCL2(std::string const& device):
		control_F9S_base(device)
	{
// 		std::cout << "MCL2" << std::endl;
// 		for(std::size_t i = 64; i < 90; ++i){
// 			try{
// 				if(i == 80) continue;
// 				std::cout << "get " << i << std::endl;
// 				get({i});
// 			}catch(...){
// 				std::cout << "error" << std::endl;
// 			}
// 		}


		write_resolution(10);

		auto resolution = read_resolution();
		if(resolution != 10){
			throw std::logic_error(
				"MCL2 init resolution error. (value is '" +
				std::to_string(resolution) + "', should be '10')"
			);
		}


		write_ramp(10);

		auto ramp = read_ramp();
		if(ramp != 10){
			throw std::logic_error(
				"MCL2 init ramp error. (value is '" +
				std::to_string(ramp) + "', should be '10')"
			);
		}


		write_leadscrew_pitch_x(10000);

		auto leadscrew_pitch_x = read_leadscrew_pitch_x();
		if(leadscrew_pitch_x != 10000){
			throw std::logic_error(
				"MCL2 init leadscrew_pitch_x error. (value is '" +
				std::to_string(leadscrew_pitch_x) + "', should be '10')"
			);
		}


		write_leadscrew_pitch_y(10000);

		auto leadscrew_pitch_y = read_leadscrew_pitch_y();
		if(leadscrew_pitch_y != 10000){
			throw std::logic_error(
				"MCL2 init leadscrew_pitch_y error. (value is '" +
				std::to_string(leadscrew_pitch_y) + "', should be '10')"
			);
		}
	}


	void control_F9S_MCL2::stop(){
		auto answer = get({{write::command, 'a'}, {read::start}});

		if(regex_search(answer, move_answer_expected)) return;

		throw std::logic_error(
			"answer after stop was '" + answer + "'"
		);
	}

	void control_F9S_MCL2::move_to(std::int64_t x, std::int64_t y){
		using namespace std::literals;

		auto answer = get({
			{write::preselection_x, x},
			{write::preselection_y, y},
			{write::command, 'r'},
			{read::start}
		}, 30s);

		if(regex_search(answer, move_answer_expected)) return;

		throw std::logic_error(
			"answer after move_to was '" + answer + "'"
		);
	}

	void control_F9S_MCL2::move_relative(std::int64_t x, std::int64_t y){
		using namespace std::literals;

		auto answer = get({
			{write::preselection_x, x},
			{write::preselection_y, y},
			{write::command, 'v'},
			{read::start}
		}, 30s);

		if(regex_search(answer, move_answer_expected)) return;

		throw std::logic_error(
			"answer after move_relative was '" + answer + "'"
		);
	}

	void control_F9S_MCL2::set_position(std::int64_t x, std::int64_t y){
		send({
			{write::absolute_position_x, x},
			{write::absolute_position_y, y}
		});
		delay();
	}

	std::array< std::int64_t, 2 > control_F9S_MCL2::position(){
		return {{
			read_x(),
			read_y()
		}};
	}

	std::array< std::int64_t, 2 > control_F9S_MCL2::preselection(){
		return {{
			read_pre_x(),
			read_pre_y()
		}};
	}


	std::regex const control_F9S_MCL2::move_answer_expected(
		"^[AD@]{3}\\-\\.$"
	);


	std::int64_t control_F9S_MCL2::read_pre_x(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::preselection_x}})
		);
	}

	std::int64_t control_F9S_MCL2::read_pre_y(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::preselection_y}})
		);
	}

	std::int64_t control_F9S_MCL2::read_x(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::absolute_position_x}})
		);
	}

	std::int64_t control_F9S_MCL2::read_y(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::absolute_position_y}})
		);
	}

	// TODO: Return a struct with parsed data, throw in error case
	std::string control_F9S_MCL2::read_status(){
		return get({{read::status}});
	}

	// TODO: Make command an enum
	char control_F9S_MCL2::read_command(){
		return get({{read::command}}).at(0);
	}

	std::int64_t control_F9S_MCL2::read_ramp(){
		return boost::lexical_cast< std::int64_t >(get({{read::ramp}}));
	}

	std::int64_t control_F9S_MCL2::read_motor_speed(){
		return boost::lexical_cast< std::int64_t >(get({{read::motor_speed}}));
	}

	std::int64_t control_F9S_MCL2::read_current_reduction(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::current_reduction}})
		);
	}

	// TODO: Return a struct with parsed data, throw in error case
	std::string control_F9S_MCL2::read_mask(){
		return get({{read::mask}});
	}

	std::int64_t control_F9S_MCL2::read_reply_delay(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::delay_time_for_replies}})
		);
	}

	std::int64_t control_F9S_MCL2::read_leadscrew_pitch_x(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::leadscrew_pitch_x}})
		);
	}

	std::int64_t control_F9S_MCL2::read_leadscrew_pitch_y(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::leadscrew_pitch_y}})
		);
	}

	std::int64_t control_F9S_MCL2::read_resolution(){
		return boost::lexical_cast< std::int64_t >(get({{read::resolution}}));
	}


	void control_F9S_MCL2::write_leadscrew_pitch_x(std::int64_t value){
		send({{write::leadscrew_pitch_x, value}});
		delay();
	}

	void control_F9S_MCL2::write_leadscrew_pitch_y(std::int64_t value){
		send({{write::leadscrew_pitch_y, value}});
		delay();
	}

	void control_F9S_MCL2::write_ramp(std::int64_t value){
		send({{write::ramp, value}});
		delay();
	}

	void control_F9S_MCL2::write_motor_speed(std::int64_t value){
		send({{write::motor_speed, value}});
		delay();
	}

	void control_F9S_MCL2::write_resolution(std::int64_t value){
		send({{write::resolution, value}});
		delay();
	}


}
