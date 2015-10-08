//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/control_F9S_base.hpp>


namespace linescan{


	control_F9S_base::control_F9S_base(std::string const& device):
		port_([this](std::string&& data){
			std::cout << "receive: '" << mask_non_print(data) << "'"
				<< std::endl;

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
		port_.send(" "); // synchronize baud rate
	}


	void control_F9S_base::send(std::vector< command > const& commands){
		std::string data;
		for(auto& command: commands){
			data += "U" + std::string(1, command.address) +
				command.data + "\r";
		}

		std::cout << "request: '" << mask_non_print(data) << "'"
			<< std::endl;

		port_.send(data);
	}


	std::string control_F9S_base::receive(){
		std::unique_lock< std::mutex > lock(mutex_);
		cv_.wait(lock);
		return receive_;
	}


}
