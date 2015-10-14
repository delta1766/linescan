//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/control_F9S_MCL3.hpp>
#include <linescan/camera.hpp>

#include <boost/type_index.hpp>

#include <png++/png.hpp>

#include <iostream>


int main()try{
	linescan::camera cam(0);
	auto image = cam.image();

	png::image< png::gray_pixel > output(image.width(), image.height());
	for(std::size_t y = 0; y < image.height(); ++y){
		for(std::size_t x = 0; x < image.width(); ++x){
			output[y][x] = image(x, y);
		}
	}
	output.write("output.png");

	return 0;

	linescan::control_F9S_MCL3 mcl3("/dev/ttyUSB0");

	std::string command;
	while(getline(std::cin, command)){
		if(command.empty()) break;

		if(command == "calib"){
			mcl3.calibrate();
		}else if(command == "stop"){
			mcl3.stop();
		}else if(command == "joy"){
			mcl3.activate_joystick();
		}else if(command == "move"){
			mcl3.move_to(100000, 100000, 100000);
		}else if(command == "mover"){
			for(std::size_t i = 0; i < 1000; ++i){
				using namespace std::literals;
				std::this_thread::sleep_for(1ms);
				mcl3.move_relative(20, 20, 20);
			}
		}else if(command == "end"){
			mcl3.move_to_end();
		}else{
			std::cout << "Unknown input" << std::endl;
		}
	}
}catch(std::exception const& e){
	std::cerr
		<< "Exit with exception: ["
		<< boost::typeindex::type_id_runtime(e).pretty_name() << "] "
		<< e.what() << std::endl;
}catch(...){
	std::cerr << "Exit with unknown exception" << std::endl;
}
