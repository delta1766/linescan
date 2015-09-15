//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/serial_port.hpp>

#include <boost/type_index.hpp>

#include <iostream>


int main()try{
	std::size_t reg[26];

	std::string buffer;
	linescan::serial_port mcl3([&](std::string&& data){
		buffer += data;

		for(;;){
			auto pos = buffer.find('U');
			if(pos == std::string::npos) return;

			buffer = buffer.substr(pos);

			pos = buffer.find('\r');
			if(pos == std::string::npos) return;

			auto command = buffer.substr(1, pos - 2);
			buffer = buffer.substr(pos);
		}
	}, "\r");

	port_.open(
		"/dev/pts/4", 9600, 8,
		flow_control::none, parity::none, stop_bits::two
	);
}catch(std::exception const& e){
	std::cerr
		<< "Exit with exception: ["
		<< boost::typeindex::type_id_runtime(e).pretty_name() << "] "
		<< e.what() << std::endl;
}catch(...){
	std::cerr << "Exit with unknown exception" << std::endl;
}
