//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/control_F9S_base.hpp>

#include <thread>


namespace linescan{


	control_F9S_base::control_F9S_base(std::string const& device):
		port_([this](std::string&& data){
			{
				std::lock_guard< std::mutex > lock(mutex_);
				receive_ = std::move(data);
			}

			cv_.notify_one();
		}, "\r")
	{
		port_.open(
			device, 9600, 8,
			flow_control::none, parity::none, stop_bits::two
		);

		// Request status
		port_.send("UF\r");

		// Read out status and potentially trash data
		receive();
	}


	std::string control_F9S_base::get(
		vector< command > const& commands, std::size_t repetitions
	){
		using namespace std::literals;
		return get(commands, 50ms, repetitions);
	}

	std::pair< std::string, bool > control_F9S_base::receive(){
		using namespace std::literals;
		return receive(50ms);
	}

	void control_F9S_base::send(vector< command > const& commands){
		std::string data;
		for(auto& command: commands){
			data += "U" + std::string(1, command.address) +
				command.data + "\r";
		}

		port_.send(data);
	}

	void control_F9S_base::send(char const* data){
		port_.send(data);
	}

	void control_F9S_base::delay()const{
		using namespace std::literals;
		std::this_thread::sleep_for(50ms);
	}

}
