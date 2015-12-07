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
#include <linescan/calc_top_distance_line.hpp>
#include <linescan/calc_calibration_lines.hpp>
#include <linescan/composed_function.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/collect_points.hpp>
#include <linescan/binarize.hpp>
#include <linescan/erode.hpp>
#include <linescan/gauss.hpp>
#include <linescan/edge.hpp>
#include <linescan/load.hpp>
#include <linescan/save.hpp>
#include <linescan/invert.hpp>
#include <linescan/calib.hpp>

#include <mitrax/norm.hpp>
#include <mitrax/operator.hpp>
#include <mitrax/output.hpp>

#include <boost/type_index.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>


int main()try{
	using namespace std::literals;
	using namespace mitrax::literals;

#ifdef HARDWARE
	linescan::control_F9S_MCL3 mcl3("/dev/ttyUSB0");
	linescan::camera cam(0);
#endif

	std::string command;
	std::size_t command_count = 0;

	auto save = [&command_count](auto& data, std::string const& name){
		std::ostringstream os;
		os << std::setfill('0') << std::setw(2) << command_count << "_";
		linescan::save(data, os.str() + name);
	};
#ifndef HARDWARE
	(void)save;
#endif

	while(getline(std::cin, command)){
		if(command.empty()) break;

#ifdef HARDWARE
		if(command == "get"){
			auto image = cam.image();
			save(image, "0_image.png");

			auto binary = binarize(image, std::uint8_t(255));
			save(binary, "1_binary.png");

			binary = erode(binary, 3);
			save(binary, "2_erode.png");

			auto line = calc_top_distance_line(binary);
			save(
				draw_top_distance_line(line, line.size(), binary.rows()),
				"3_line.png"
			);
		}else if(command == "calib_"){
			mcl3.calibrate();
		}else if(command == "stop"){
			mcl3.stop();
		}else if(command == "joy"){
			mcl3.activate_joystick();
		}else if(command == "move"){
			mcl3.move_to(100000, 100000, 100000);
		}else if(command == "image"){
				std::cout << cam.exposure_in_ms_min() << std::endl;
				std::cout << cam.exposure_in_ms_max() << std::endl;
				std::cout << cam.exposure_in_ms() << std::endl;
				cam.set_exposure(cam.exposure_in_ms_max());
				std::this_thread::sleep_for(100ms);
				std::cout << cam.exposure_in_ms() << std::endl;

				auto calib = cam.image();

				save(calib, "image.png");
		}else if(command == "measure"){
			for(std::size_t i = 0; i < 10; ++i){
				std::this_thread::sleep_for(100ms);

				std::cout << "Get calib " << i << std::endl;
				std::cin.get();

				auto calib = cam.image();

				{
					std::ostringstream os;
					os
						<< "calib" << std::setfill('0')
						<< std::setw(4) << i << ".png";
					linescan::save(calib, os.str());
				}

				std::cout << "Get laser " << i << std::endl;
				std::cin.get();

				auto laser = cam.image();

				{
					std::ostringstream os;
					os
						<< "laser" << std::setfill('0')
						<< std::setw(4) << i << ".png";
					linescan::save(laser, os.str());
				}

				mcl3.move_relative(0, -1000, 0);
			}

			mcl3.move_relative(0, 10000, 0);
		}else if(command == "end"){
			mcl3.move_to_end();
		}else
#endif
		if(command == "calib"){
			linescan::calib();
		}else{
			std::cout << "Unknown input" << std::endl;
		}

		++command_count;
	}
}catch(std::exception const& e){
	std::cerr
		<< "Exit with exception: ["
		<< boost::typeindex::type_id_runtime(e).pretty_name() << "] "
		<< e.what() << std::endl;
}catch(...){
	std::cerr << "Exit with unknown exception" << std::endl;
}
