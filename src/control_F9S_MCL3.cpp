//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/control_F9S_MCL3.hpp>


namespace linescan{


	control_F9S_MCL3::control_F9S_MCL3(std::string const& device):
		control_F9S_base(device),
		joystick_(false)
#ifndef MCL
		,pre_x_(0)
		,pre_y_(0)
		,pre_z_(0)
		,x_(0)
		,y_(0)
		,z_(0)
#endif
	{
#ifdef MCL
		write_resolution(10);

		auto resolution = read_resolution();
		if(resolution != 10){
			throw std::logic_error(
				"MCL3 init resolution error. (value is '" +
				std::to_string(resolution) + "', should be '10')"
			);
		}


		write_ramp(10);

		auto ramp = read_ramp();
		if(ramp != 10){
			throw std::logic_error(
				"MCL3 init ramp error. (value is '" +
				std::to_string(ramp) + "', should be '10')"
			);
		}


		write_leadscrew_pitch_x(10000);

		auto leadscrew_pitch_x = read_leadscrew_pitch_x();
		if(leadscrew_pitch_x != 10000){
			throw std::logic_error(
				"MCL3 init leadscrew_pitch_x error. (value is '" +
				std::to_string(leadscrew_pitch_x) + "', should be '10000')"
			);
		}


		write_leadscrew_pitch_y(10000);

		auto leadscrew_pitch_y = read_leadscrew_pitch_y();
		if(leadscrew_pitch_y != 10000){
			throw std::logic_error(
				"MCL3 init leadscrew_pitch_y error. (value is '" +
				std::to_string(leadscrew_pitch_y) + "', should be '10000')"
			);
		}


		write_leadscrew_pitch_z(10000);

		auto leadscrew_pitch_z = read_leadscrew_pitch_z();
		if(leadscrew_pitch_z != 10000){
			throw std::logic_error(
				"MCL3 init leadscrew_pitch_z error. (value is '" +
				std::to_string(leadscrew_pitch_z) + "', should be '10000')"
			);
		}
#endif
	}


	std::string control_F9S_MCL3::name()const{
		return "MCL3";
	}

	void control_F9S_MCL3::activate_joystick(){
#ifdef MCL
		std::lock_guard< std::mutex > lock(joystick_mutex_);
		send({{write::command, 'j'}, {read::start}});
#endif
		joystick_ = true;
	}

	void control_F9S_MCL3::deactivate_joystick(){
#ifdef MCL
		std::lock_guard< std::mutex > lock(joystick_mutex_);
		if(!joystick_) return;

		send("j");
#endif
		joystick_ = false;

#ifdef MCL
		auto answer = receive();
		if(answer.second) return;
		if(regex_search(answer.first, move_answer_expected)) return;

		throw std::logic_error(
			"answer after deactivating joystick was '" + answer.first + "'"
		);
#endif
	}

	void control_F9S_MCL3::calibrate(){
		using namespace std::literals;

		deactivate_joystick();

#ifdef MCL
		// TODO: respect mask
		auto answer = get({{write::command, 'c'}, {read::start}}, 30s);
		if(answer == "AAA-.") return;

		throw std::logic_error(
			"answer after calibrate was '" + answer +
			"', expected was 'AAA-.'"
		);
#else
		x_ = -500000;
		y_ = -500000;
		z_ = -500000;
#endif
	}

	void control_F9S_MCL3::move_to_end(){
		using namespace std::literals;

		deactivate_joystick();

#ifdef MCL
		// TODO: respect mask
		auto answer = get({{write::command, 'l'}, {read::start}}, 30s);
		if(answer == "DDD-.") return;

		throw std::logic_error(
			"answer after move to end was '" + answer +
			"', expected was 'DDD-.'"
		);
#else
		x_ = 500000;
		y_ = 500000;
		z_ = 500000;
#endif
	}

	void control_F9S_MCL3::stop(){
		deactivate_joystick();

#ifdef MCL
		auto answer = get({{write::command, 'a'}, {read::start}});
		if(regex_search(answer, move_answer_expected)) return;

		throw std::logic_error(
			"answer after stop was '" + answer + "'"
		);
#endif
	}

	void control_F9S_MCL3::set_position(
		std::int64_t x, std::int64_t y, std::int64_t z
	){
		deactivate_joystick();

#ifdef MCL
		send({
			{write::absolute_position_x, x},
			{write::absolute_position_y, y},
			{write::absolute_position_z, z}
		});
		delay();
#else
		x_ = x;
		y_ = y;
		z_ = z;
#endif
	}

	void control_F9S_MCL3::move_to(
		std::int64_t x, std::int64_t y, std::int64_t z
	){
		using namespace std::literals;

		deactivate_joystick();

#ifdef MCL
		auto answer = get({
			{write::preselection_x, x},
			{write::preselection_y, y},
			{write::preselection_z, z},
			{write::command, 'r'},
			{read::start}
		}, 30s);

		if(regex_search(answer, move_answer_expected)) return;

		throw std::logic_error(
			"answer after move_to was '" + answer + "'"
		);
#else
		pre_x_ = x;
		pre_y_ = y;
		pre_z_ = z;
		x_ = x;
		y_ = y;
		z_ = z;
#endif
	}

	void control_F9S_MCL3::move_relative(
		std::int64_t x, std::int64_t y, std::int64_t z
	){
		using namespace std::literals;

		deactivate_joystick();

#ifdef MCL
		auto answer = get({
			{write::preselection_x, x},
			{write::preselection_y, y},
			{write::preselection_z, z},
			{write::command, 'v'},
			{read::start}
		}, 30s);

		if(regex_search(answer, move_answer_expected)) return;

		throw std::logic_error(
			"answer after move_relative was '" + answer + "'"
		);
#else
		x_ += x;
		y_ += y;
		z_ += z;
		pre_x_ = x_;
		pre_y_ = y_;
		pre_z_ = z_;
#endif
	}

	std::array< std::int64_t, 3 > control_F9S_MCL3::position(){
		return {{ read_x(), read_y(), read_z() }};
	}

	std::array< std::int64_t, 3 > control_F9S_MCL3::preselection(){
#ifdef MCL
		return {{ read_pre_x(), read_pre_y(), read_pre_z() }};
#else
		return {{ pre_x_, pre_y_, pre_z_ }};
#endif
	}


	std::regex const control_F9S_MCL3::move_answer_expected(
		"^[AD@]{3}\\-\\.$"
	);


	std::int64_t control_F9S_MCL3::read_pre_x(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::preselection_x}})
		);
	}

	std::int64_t control_F9S_MCL3::read_pre_y(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::preselection_y}})
		);
	}

	std::int64_t control_F9S_MCL3::read_pre_z(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::preselection_z}})
		);
	}

	std::int64_t control_F9S_MCL3::read_x(){
#ifdef MCL
		return boost::lexical_cast< std::int64_t >(
			get({{read::absolute_position_x}})
		);
#else
		return x_;
#endif

	}

	std::int64_t control_F9S_MCL3::read_y(){
#ifdef MCL
		return boost::lexical_cast< std::int64_t >(
			get({{read::absolute_position_y}})
		);
#else
		return y_;
#endif

	}

	std::int64_t control_F9S_MCL3::read_z(){
#ifdef MCL
		return boost::lexical_cast< std::int64_t >(
			get({{read::absolute_position_z}})
		);
#else
		return z_;
#endif

	}

	// TODO: Return a struct with parsed data, throw in error case
	std::string control_F9S_MCL3::read_status(){
		return get({{read::status}});
	}

	// TODO: Make command an enum
	char control_F9S_MCL3::read_command(){
		return get({{read::command}}).at(0);
	}

	std::int64_t control_F9S_MCL3::read_ramp(){
		return boost::lexical_cast< std::int64_t >(get({{read::ramp}}));
	}

	std::int64_t control_F9S_MCL3::read_motor_speed(){
		return boost::lexical_cast< std::int64_t >(get({{read::motor_speed}}));
	}

	std::int64_t control_F9S_MCL3::read_current_reduction(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::current_reduction}})
		);
	}

	// TODO: Return a struct with parsed data, throw in error case
	std::string control_F9S_MCL3::read_mask(){
		return get({{read::mask}});
	}

	std::int64_t control_F9S_MCL3::read_reply_delay(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::delay_time_for_replies}})
		);
	}

	std::int64_t control_F9S_MCL3::read_leadscrew_pitch_x(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::leadscrew_pitch_x}})
		);
	}

	std::int64_t control_F9S_MCL3::read_leadscrew_pitch_y(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::leadscrew_pitch_y}})
		);
	}

	std::int64_t control_F9S_MCL3::read_leadscrew_pitch_z(){
		return boost::lexical_cast< std::int64_t >(
			get({{read::leadscrew_pitch_z}})
		);
	}

	std::int64_t control_F9S_MCL3::read_resolution(){
		return boost::lexical_cast< std::int64_t >(get({{read::resolution}}));
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
