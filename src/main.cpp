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
#include <linescan/linear_function.hpp>

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
			mcl3.move_relative(0, 0, 1000);

// 			double from = 0;
// 			double to = cam.width() * cam.pixel_size_in_um();
			std::vector< std::pair< double, double > > lines;
			for(std::size_t i = 0; i < 20; ++i){
				using namespace std::literals;

				auto image = cam.image();

				{
					png::image< png::gray_pixel > output(image.width(), image.height());
					for(std::size_t y = 0; y < image.height(); ++y){
						for(std::size_t x = 0; x < image.width(); ++x){
							output[y][x] = image(x, y);
						}
					}
					std::ostringstream os;
					os << "img" << std::setw(4) << std::setfill('0') << i << ".png";
					output.write(os.str());
				}

				auto pixel_line = calc_line(image);

				std::vector< linescan::point< double > > line;
				for(std::size_t i = 0; i < pixel_line.size(); ++i){
					if(pixel_line[i] == 0) continue;

					line.emplace_back(
						i * cam.pixel_size_in_um(),
						pixel_line[i] * cam.pixel_size_in_um()
					);
				}

				auto f = linescan::fit_linear_function< double >(
					line.begin(),
					line.end()
				);

// 				lines.emplace_back(f(from), f(to));

				std::cout << "(0; " << f(0) << ") - (100; " << f(100) << ")"
					<< std::endl;

				{
					png::image< png::gray_pixel > output(image.width(), image.height());
					for(std::size_t x = 0; x < image.width(); ++x){
						if(pixel_line[x] == 0) continue;
						auto pos1 = static_cast< std::size_t >(pixel_line[x]);
						auto pos2 = static_cast< std::size_t >(pixel_line[x] - 0.5f);
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
				}

// 				png::image< png::gray_pixel > output(image.width(), image.height());
// 				for(std::size_t y = 0; y < image.height(); ++y){
// 					for(std::size_t x = 0; x < image.width(); ++x){
// 						output[y][x] = image(x, y);
// 					}
// 				}
// 				std::ostringstream os;
// 				os << "out" << std::setw(4) << std::setfill('0') << i << ".png";
// 				output.write(os.str());

				mcl3.move_relative(0, 0, -100);
			}

// 			double diff_100um = 0;
// 			for(std::size_t i = 0; i < lines.size() - 1; ++i){
// 				diff_100um +=
// 					lines[i + 1].first - lines[i].first + 
// 					lines[i + 1].second - lines[i].second;
// 			}
// 			diff_100um /= (lines.size() - 1) * 2;

			mcl3.move_relative(0, 0, 1000);
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
