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
#include <linescan/median.hpp>
#include <linescan/edge.hpp>
#include <linescan/load.hpp>
#include <linescan/save.hpp>
#include <linescan/invert.hpp>
#include <linescan/calib.hpp>
#include <linescan/main_window.hpp>

#include <mitrax/regions.hpp>
#include <mitrax/norm.hpp>
#include <mitrax/operator.hpp>
#include <mitrax/output.hpp>
#include <mitrax/point_io.hpp>

#include <boost/type_index.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <cmath>


int main(int argc, char** argv)try{
	QApplication app(argc, argv);

	linescan::main_window window;
	window.showMaximized();

	return app.exec();

	using namespace std::literals;
	using namespace mitrax::literals;

#ifdef MCL
	linescan::control_F9S_MCL3 mcl3("/dev/ttyUSB0");
#endif
#ifdef CAM
	linescan::camera cam(0);

	std::uint32_t pixelclock;
	double framerate;
	double exposure_in_ms;
	std::size_t gain_in_percent;
	bool gain_boost;


	std::tie(
		pixelclock, framerate, exposure_in_ms, gain_in_percent, gain_boost
	) = cam.get_light_params();

	auto set_default_light = [
		&cam, pixelclock, framerate, exposure_in_ms, gain_in_percent, gain_boost
	](){
		cam.set_light_params(
			pixelclock, framerate, exposure_in_ms, gain_in_percent, gain_boost
		);
		cam.image();
	};

	auto set_max_light = [&cam](){
		auto framerate = cam.framerate_max();
		cam.set_framerate(framerate);
		auto pixelclock = cam.pixelclock_min();
		cam.set_pixelclock(pixelclock);
		auto exposure = cam.exposure_in_ms_max();
		cam.set_exposure(exposure);
		cam.set_gain(100);
		cam.set_gain_boost(true);
		cam.image();
	};
#endif

	std::string command;
	std::size_t command_count = 0;

	auto save = [&command_count](auto const& data, std::string const& name){
		std::ostringstream os;
		os << std::setfill('0') << std::setw(2) << command_count << "_";
		linescan::save(data, os.str() + name);
	};
	(void)save;

	while(getline(std::cin, command)){
		if(command.empty()) break;

		if(command == "get"){
#ifdef CAM
			set_default_light();
			auto image = cam.image();
#else
			auto image = linescan::load("simulation/laser.png");
#endif
			save(image, "0_image.png");

			auto binary = linescan::binarize(image, std::uint8_t(255));
			save(binary, "1_binary.png");

			binary = linescan::erode(binary, 3);
			save(binary, "2_erode.png");

			auto line = linescan::calc_top_distance_line(binary);
			save(
				linescan::draw_top_distance_line_student(
					line, line.size(), binary.rows()
				), "3_line.png"
			);
		}else if(command == "cycle"){
#ifdef CAM
			set_max_light();
			auto image = cam.image();
#else
			auto image = linescan::load("simulation/cycle.png");
#endif

// 			auto size = image.dims();
			save(image, "0_image.png");

// 			image = linescan::median(image, mitrax::dims(5, 5));
// 			save(image, "1_median.png");
// 
// 			image = linescan::gauss< 5 >(image, 0.7);
// 			save(image, "2_gauss.png");

			auto region = mitrax::calc_regions(
				[](auto const& m){
					auto sum = std::accumulate(m.begin(), m.end(), 0.);
					return static_cast< std::uint8_t >(
						sum / (static_cast< std::size_t >(m.cols()) * m.rows())
					);
				},
				mitrax::dims(96, 96),
				mitrax::dims(48, 48),
				image
			);

			auto result = mitrax::apply_regions(
				[](boost::container::vector< std::uint8_t > const& regions){
					return static_cast< std::uint8_t >(regions.size() * 30);
				},
				mitrax::dims(96, 96),
				region,
				image
			);
			save(result, "4_result.png");

// 			auto edge = mitrax::pass_in(
// 				size,
// 				linescan::edge_amplitude(image),
// 				0
// 			);
// 			save(edge, "3_edge.png");

// 			auto binary = linescan::binarize(image, std::uint8_t(50));
// 			save(binary, "2_binary.png");
// 
// 			binary = linescan::erode(binary, 3, true);
// 			save(binary, "3_erode.png");
		}else
#ifdef MCL
		if(command == "calib_"){
			mcl3.calibrate();
		}else if(command == "stop"){
			mcl3.stop();
		}else if(command == "joy"){
			mcl3.activate_joystick();
		}else if(command == "move"){
			mcl3.move_to(100000, 100000, 100000);
		}else
#endif
#ifdef CAM
		if(command == "image"){
				std::cout << cam.exposure_in_ms_min() << std::endl;
				std::cout << cam.exposure_in_ms_max() << std::endl;
				std::cout << cam.exposure_in_ms() << std::endl;
				cam.set_exposure(cam.exposure_in_ms_max());
				std::this_thread::sleep_for(100ms);
				std::cout << cam.exposure_in_ms() << std::endl;

				auto calib = cam.image();

				save(calib, "image.png");
		}else
#endif
#ifdef HARDWARE
		if(command == "measure"){
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
					save(calib, os.str());
				}

				std::cout << "Get laser " << i << std::endl;
				std::cin.get();

				auto laser = cam.image();

				{
					std::ostringstream os;
					os
						<< "laser" << std::setfill('0')
						<< std::setw(4) << i << ".png";
					save(laser, os.str());
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
