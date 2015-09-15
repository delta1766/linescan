//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/control_F9S_MCL2.hpp>
#include <linescan/control_F9S_MCL3.hpp>

#include <boost/type_index.hpp>

#include <iostream>


int main()try{
	linescan::control_F9S_MCL2 mcl2("/dev/ttyUSB1");
	linescan::control_F9S_MCL3 mcl3("/dev/ttyUSB0");

	std::string command;
	while(getline(std::cin, command)){
		if(command.empty()) break;

		if(command == "rs"){
// 			mcl2.read_status();
			mcl3.read_status();
		}else if(command == "calib"){
// 			mcl2.start_calibration();
			mcl3.start_calibration();
		}else if(command == "x"){
			mcl3.read_x();
		}else if(command == "y"){
			mcl3.read_y();
		}else if(command == "z"){
			mcl3.read_z();
		}else if(command == "s"){
			mcl3.start();
		}else if(command == "b"){
			mcl3.stop();
		}else if(command == "d"){
			mcl3.distance();
		}else{
			std::cout << "Unknown input" << std::endl;
		}
	}
}catch(std::exception const& e){
	std::cerr << "Exit with exception: [" << boost::typeindex::type_id_runtime(e).pretty_name() << "] " << e.what() << std::endl;
}catch(...){
	std::cerr << "Exit with unknown exception" << std::endl;
}
