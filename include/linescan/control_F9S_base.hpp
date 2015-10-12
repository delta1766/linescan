//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__control_F9S_base__hpp_INCLUDED_
#define _linescan__control_F9S_base__hpp_INCLUDED_

#include "serial_port.hpp"
#include "mask_non_print.hpp"

#include <iostream>
#include <condition_variable>
#include <mutex>


namespace linescan{


	class control_F9S_base{
	public:
		control_F9S_base(std::string const& device);


	protected:
		struct command{
			command(std::uint8_t address, std::string&& data = ""):
				address(address), data(std::move(data)) {}

			command(std::uint8_t address, char data):
				address(address), data(1, data) {}

			command(std::uint8_t address, std::int64_t data):
				address(address), data(std::to_string(data)) {}

			std::uint8_t address;
			std::string data;
		};


		void send(std::vector< command > const& commands);

		std::string receive();


	private:
		std::mutex mutex_;
		std::condition_variable cv_;

		std::string receive_;

		serial_port port_;
	};


}


#endif
