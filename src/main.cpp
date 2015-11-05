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
#include <linescan/point_io.hpp>

#include <boost/type_index.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>


int main()try{
	using namespace std::literals;

// 	linescan::control_F9S_MCL3 mcl3("/dev/ttyUSB0");
// 
// 	linescan::camera cam(0);

	std::string command;
	while(getline(std::cin, command)){
		if(command.empty()) break;

// 		if(command == "calib"){
// 			mcl3.calibrate();
// 		}else if(command == "stop"){
// 			mcl3.stop();
// 		}else if(command == "joy"){
// 			mcl3.activate_joystick();
// 		}else if(command == "move"){
// 			mcl3.move_to(100000, 100000, 100000);
// 		}else if(command == "image"){
// 			cam.set_exposure(cam.exposure_in_ms_max());
// 
// 			std::this_thread::sleep_for(100ms);
// 
// 			auto image = cam.image();
// 
// 			save(image, "01_image.png");
// 
// 			image = linescan::gauss< 5 >(image, 0.7);
// 
// 			save(image, "02_gauss.png");
// 
// 			auto binary = linescan::binarize(image, std::uint8_t(255));
// 
// 			save(binary, "03_binary.png");
// 
// 			binary = linescan::erode(binary, 5);
// 
// 			save(binary, "04_erode.png");
// 
// 			auto line = linescan::calc_line(binary);
// 
// 			save(line, binary.height(), "05_line.png");
// 		}else if(command == "measure"){
// 			mcl3.move_relative(0, 0, 1000);
// 
// 			auto diff =
// 				(cam.exposure_in_ms_max() - cam.exposure_in_ms_min()) / 9;
// 
// // 			double from = 0;
// // 			double to = cam.width() * cam.pixel_size_in_um();
// 			vector< std::pair< double, double > > lines;
// 			for(std::size_t i = 0; i < 20; ++i){
// 				for(std::size_t n = 5; n < 10; ++n){
// 					cam.set_exposure(cam.exposure_in_ms_min() + n * diff);
// 
// 					std::this_thread::sleep_for(100ms);
// 
// 					auto image = cam.image();
// 
// 					{
// 						std::ostringstream os;
// 						os
// 							<< "img" << std::setfill('0')
// 							<< std::setw(4) << i << "_"
// 							<< std::setw(4) << n << ".png";
// 						save(image, os.str());
// 					}
// 
// 
// 					auto pixel_line = calc_line(image);
// 
// 					std::ostringstream os;
// 					os
// 						<< "line" << std::setfill('0')
// 						<< std::setw(4) << i << "_"
// 						<< std::setw(4) << n << ".png";
// 					save(pixel_line, image.height(), os.str());
// 
// 
// 					vector< linescan::point< double > > line;
// 					for(std::size_t i = 0; i < pixel_line.size(); ++i){
// 						if(pixel_line[i] == 0) continue;
// 
// 						line.emplace_back(
// 							i * cam.pixel_size_in_um(),
// 							pixel_line[i] * cam.pixel_size_in_um()
// 						);
// 					}
// 
// 					auto f = linescan::fit_linear_function< double >(
// 						line.begin(),
// 						line.end()
// 					);
// 
// 	// 				lines.emplace_back(f(from), f(to));
// 
// 					std::cout << "(0; " << f(0) << ") - (100; " << f(100) << ")"
// 						<< std::endl;
// 				}
// 
// 				mcl3.move_relative(0, 0, -100);
// 			}
// 
// // 			double diff_100um = 0;
// // 			for(std::size_t i = 0; i < lines.size() - 1; ++i){
// // 				diff_100um +=
// // 					lines[i + 1].first - lines[i].first + 
// // 					lines[i + 1].second - lines[i].second;
// // 			}
// // 			diff_100um /= (lines.size() - 1) * 2;
// 
// 			mcl3.move_relative(0, 0, 1000);
// 		}else if(command == "line"){
// 			cam.set_exposure(cam.exposure_in_ms_max());
// 			std::this_thread::sleep_for(100ms);
// 
// 			std::ofstream raw("raw.txt");
// 			std::ofstream os("line.txt");
// 
// 			mcl3.move_relative(0, 0, 10000);
// 			for(std::size_t i = 0; i < 500; ++i){
// 				mcl3.move_relative(0, 300, 0);
// 
// 				vector< std::pair< std::size_t, std::size_t > > lines;
// 				for(std::size_t n = 0; n < 60; ++n){
// 					auto image = cam.image();
// 					auto binary = linescan::binarize(image, std::uint8_t(255));
// 					binary = linescan::erode(binary, 5);
// 					auto line = linescan::calc_line(binary);
// 
// 					// add longest contiguous stretch
// 					std::size_t from = 0;
// 					std::size_t to = 0;
// 
// 					std::size_t max_length = 0;
// 					std::size_t start = 0;
// 					for(std::size_t i = 0; i < line.size(); ++i){
// 						if(!line[i]){
// 							if(start == 0) continue;
// 
// 							auto length = i - start;
// 							if(length > max_length){
// 								max_length = length;
// 
// 								from = start;
// 								to = i;
// 							}
// 
// 							start = 0;
// 						}else{
// 							if(start != 0) continue;
// 
// 							start = i;
// 						}
// 					}
// 
// 					lines.emplace_back(from, to);
// 
// 					mcl3.move_relative(0, -10, 0);
// 				}
// 
// 				for(auto line: lines){
// 					raw << line.first << '\t' << line.second << std::endl;
// 				}
// 				raw << std::endl;
// 
// 				// sort by start
// 				std::sort(lines.begin(), lines.end(), [](auto a, auto b){
// 					return a.first < b.first;
// 				});
// 				lines.erase(lines.begin(), lines.begin() + 10);
// 				lines.erase(lines.end() - 10, lines.end());
// 
// 				// sort by end
// 				std::sort(lines.begin(), lines.end(), [](auto a, auto b){
// 					return a.second < b.second;
// 				});
// 				lines.erase(lines.begin(), lines.begin() + 10);
// 				lines.erase(lines.end() - 10, lines.end());
// 
// 				// calc average of the rest
// 				std::pair< double, double > length(0, 0);
// 				for(auto line: lines){
// 					length.first += line.first;
// 					length.second += line.second;
// 				}
// 				length.first /= lines.size();
// 				length.second /= lines.size();
// 
// 				os << length.first << '\t' << length.second << std::endl;
// 
// 				mcl3.move_relative(0, 300, 0);
// 
// 				mcl3.move_relative(0, 0, -200);
// 			}
// 
// 			mcl3.move_relative(0, 0, 10000);
// 		}else if(command == "end"){
// 			mcl3.move_to_end();
		/*}else */if(command == "laser"){
			linescan::bitmap< std::uint8_t > image;
			linescan::load(image, "simulation/real2_laser.png");

			auto binary = linescan::binarize(image, std::uint8_t(255));

			linescan::save(binary, "01_binary.png");

			binary = linescan::erode(binary, 3);

			linescan::save(binary, "02_erode.png");

			auto line = linescan::calc_top_distance_line(binary);

			linescan::save(line, binary.height(), "03_line.png");

			auto lines = linescan::calc_calibration_lines(line, 15);
			auto const& line1 = lines.first;
			auto const& line2 = lines.second;

			auto l1 = linescan::fit_linear_function< float >(line1.begin(), line1.end());
			auto l2 = linescan::fit_linear_function< float >(line2.begin(), line2.end());

			auto calib_line = make_composed_function(l1, intersection(l1, l2), l2);

			{
				linescan::bitmap< std::uint8_t > lines(binary.width(), binary.height());
				linescan::draw(lines, calib_line);
				linescan::save(lines, "04_calib_line.png");
			}
		}else if(command == "ref"){
			linescan::bitmap< std::uint8_t > image;
			linescan::load(image, "simulation/real2_ref.png");

			auto binary = linescan::binarize(image, std::uint8_t(20));

			linescan::save(binary, "11_binary.png");

			binary = linescan::erode(binary, 3);

			linescan::save(binary, "12_erode.png");

			binary = linescan::invert(binary);

			linescan::save(binary, "13_invert.png");

			auto point_and_counts = linescan::collect_points(binary);

			if(point_and_counts.size() < 8){
				throw std::logic_error(
					"not 8 points fount in reference image"
				);
			}else if(point_and_counts.size() > 8){
				// Sort by size
				std::sort(point_and_counts.begin(), point_and_counts.end(),
					[](auto& a, auto& b){
						return a.second < b.second;
					});

				// keep the 8 biggest, remove the rest
				point_and_counts.erase(
					point_and_counts.begin() + 8,
					point_and_counts.end()
				);
			}

			linescan::vector< linescan::point< float > > ref_points;
			ref_points.reserve(8);
			for(auto& v: point_and_counts) ref_points.push_back(v.first);

			for(auto& v: ref_points) std::cout << v << std::endl;

			{
				linescan::bitmap< std::uint8_t > image(binary.width(), binary.height());
				linescan::draw(image, ref_points);
				linescan::save(image, "14_ref.png");
			}
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
