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
#include <linescan/calc_line.hpp>

#include <boost/type_index.hpp>

#include <png++/png.hpp>

#include <iostream>
#include <sstream>
#include <iomanip>


int main()try{
	linescan::control_F9S_MCL3 mcl3("/dev/ttyUSB0");

	linescan::camera cam(0);

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
		}else if(command == "measure"){
			for(std::size_t i = 0; i < 100; ++i){
				using namespace std::literals;

				auto image = cam.image();

				auto line = calc_line(image);

				png::image< png::gray_pixel > output(image.width(), image.height());
				for(std::size_t x = 0; x < image.width(); ++x){
					auto pos1 = static_cast< std::size_t >(line[x]);
					auto pos2 = static_cast< std::size_t >(line[x] - 0.5f);
					if(pos1 == pos2){
						output[pos1][x] = 255;
					}else{
						output[pos1][x] = 128;
						output[pos2][x] = 128;
					}
				}
				std::ostringstream os;
				os << "line" << std::setw(4) << std::setfill('0') << i << ".png";
				output.write(os.str());

// 				png::image< png::gray_pixel > output(image.width(), image.height());
// 				for(std::size_t y = 0; y < image.height(); ++y){
// 					for(std::size_t x = 0; x < image.width(); ++x){
// 						output[y][x] = image(x, y);
// 					}
// 				}
// 				std::ostringstream os;
// 				os << "out" << std::setw(4) << std::setfill('0') << i << ".png";
// 				output.write(os.str());

				mcl3.move_relative(0, -100, 0);
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
