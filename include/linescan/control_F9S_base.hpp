//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan_control_F9S_base_hpp_INCLUDED_
#define _linescan_control_F9S_base_hpp_INCLUDED_

#include "serial_port.hpp"
#include "mask_non_print.hpp"

#include <iostream>


namespace linescan{


	class control_F9S_base{
	public:
		control_F9S_base(std::string const& device):
			port_([](std::string const& data){
				std::cout << "read: '" << mask_non_print(data) << "'" << std::endl;
			}, "\r")
		{
			port_.open(device, 9600, 8, flow_control::none, parity::none, stop_bits::two);
			port_.send(" ");
// 			port_.send("U\x11""1\r");
		}


	protected:
		struct command{
			command(std::uint8_t address, std::string&& data = ""):
				address(address), data(std::move(data)) {}

			command(std::uint8_t address, std::uint8_t data):
				address(address), data(1, data) {}

			std::uint8_t address;
			std::string data;
		};


		void send(std::vector< command > const& commands){
			std::string data;
			for(auto& command: commands){
				data += "U" + std::string(1, command.address) + command.data + "\r";
			}
			std::cout << "send: '" << mask_non_print(data) << "'" << std::endl;
			port_.send(data);
		}


	private:
		serial_port port_;
	};


}


#endif
