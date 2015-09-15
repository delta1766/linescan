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
	using namespace linescan;

	linescan::serial_port mcl3([&](std::string&& data){
		auto pos = data.find('U');
		if(pos == std::string::npos) return;

		std::uint8_t command = data[pos + 1];
		data = data.substr(pos + 2);

		std::cout
			<< static_cast< unsigned >(command) << ": '" << data << "'"
			<< std::endl;

		switch(command){
			// read commands
			case 255:
			case 0:
				
			break;
			case 1:
				
			break;
			case 2:
				
			break;
			case 3:
				
			break;
			case 4:
				
			break;
			case 5:
				
			break;
			case 6:
				
			break;
			case 7:
				
			break;
			case 8:
				
			break;
			case 9:
				
			break;
			case 10:
				
			break;
			case 11:
				
			break;
			case 12:
				
			break;
			case 17:
				
			break;
			case 21:
				
			break;
			case 22:
				
			break;
			case 23:
				
			break;
			case 25:
				
			break;
			// write commands
			case 64:
				
			break;
			case 65:
				
			break;
			case 66:
				
			break;
			case 67:
				
			break;
			case 68:
				
			break;
			case 69:
				
			break;
			case 70:
				
			break;
			case 71:
				
			break;
			case 72:
				
			break;
			case 73:
				
			break;
			case 74:
				
			break;
			case 75:
				
			break;
			case 76:
				
			break;
			case 80:
				
			break;
			case 81:
				
			break;
			case 85:
				
			break;
			case 86:
				
			break;
			case 87:
				
			break;
			case 89:
				
			break;
			// unknown
			default:
				std::cerr
					<< "Unknown command " << static_cast< unsigned >(command)
					<< std::endl;
		}
		mcl3.send("");
	}, "\r");

	mcl3.open(
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
