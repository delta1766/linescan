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
#include <linescan/binarize.hpp>
#include <linescan/erode.hpp>
#include <linescan/gauss.hpp>
#include <linescan/edge.hpp>
#include <linescan/normelize_to_uint8.hpp>

#include <boost/type_index.hpp>

#include <png++/png.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>


void load(linescan::bitmap< std::uint8_t >& image, std::string const& name){
	std::cout << "read " << name << std::endl;
	png::image< png::gray_pixel > output;
	output.read(name);
	image.resize(output.get_width(), output.get_height());
	for(std::size_t y = 0; y < output.get_height(); ++y){
		for(std::size_t x = 0; x < output.get_width(); ++x){
			image(x, y) = output[y][x];
		}
	}
}


void save(linescan::bitmap< std::uint8_t > const& image, std::string const& name){
	std::cout << "write " << name << std::endl;
	png::image< png::gray_pixel > output(image.width(), image.height());
	for(std::size_t y = 0; y < image.height(); ++y){
		for(std::size_t x = 0; x < image.width(); ++x){
			output[y][x] = image(x, y);
		}
	}
	output.write(name);
}

void save(linescan::bitmap< bool > const& image, std::string const& name){
	std::cout << "write " << name << std::endl;
	png::image< png::packed_gray_pixel< 1 > > output(image.width(), image.height());
	for(std::size_t y = 0; y < image.height(); ++y){
		for(std::size_t x = 0; x < image.width(); ++x){
			output[y][x] = image(x, y);
		}
	}
	output.write(name);
}

void save(linescan::bitmap< std::int32_t > const& image, std::string const& name){
	save(linescan::normelize_to_uint8(image), name);
}

void draw(
	linescan::bitmap< std::uint8_t >& image,
	linescan::point< float > const& point
){
	auto x = static_cast< int >(point.x());
	auto y = static_cast< int >(point.y());

	auto dx = point.x() - x;
	auto dy = point.y() - y;

	auto draw = [&image](int x, int y, float v){
		if(
			x < 0 || y < 0 ||
			x >= static_cast< int >(image.width()) ||
			y >= static_cast< int >(image.height())
		) return;

		auto r = image(x, y) + v;
		image(x, y) = static_cast< std::uint8_t >(r > 255 ? 255 : r);
	};

	draw(x, y, 255 * (1 - dx) * (1 - dy));
	draw(x + 1, y, 255 * dx * (1 - dy));
	draw(x, y + 1, 255 * (1 - dx) * dy);
	draw(x + 1, y + 1, 255 * dx * dy);
}

void draw(
	linescan::bitmap< std::uint8_t >& image,
	std::vector< linescan::point< float > > const& line
){
	for(std::size_t i = 0; i < line.size(); ++i){
		draw(image, line[i]);
	}
}

void draw(
	linescan::bitmap< std::uint8_t >& image,
	linescan::linear_function< float > const& fn
){
	std::vector< linescan::point< float > > line;
	for(std::size_t i = 0; i < image.width(); ++i){
		auto y = fn(i);
		if(y < 0) continue;
		if(y >= image.height()) continue;
		line.emplace_back(i, y);
	}
	draw(image, line);
}

linescan::bitmap< std::uint8_t > draw_top_distance_line(
	std::vector< float > const& line,
	std::size_t width,
	std::size_t height
){
	linescan::bitmap< std::uint8_t > image(width, height);

	std::vector< linescan::point< float > > point_line;
	for(std::size_t i = 0; i < line.size(); ++i){
		if(line[i] == 0) continue;
		point_line.emplace_back(i, line[i]);
	}

	draw(image, point_line);
	return image;
}

void save(std::vector< float > const& line, std::size_t height, std::string const& name){
	save(draw_top_distance_line(line, line.size(), height), name);
}


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
// 			std::vector< std::pair< double, double > > lines;
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
// 					std::vector< linescan::point< double > > line;
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
// 				std::vector< std::pair< std::size_t, std::size_t > > lines;
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
		/*}else */if(command == "load"){
			linescan::bitmap< std::uint8_t > image;
			load(image, "simulation/real_laser.png");

			auto binary = linescan::binarize(image, std::uint8_t(255));

			save(binary, "03_binary.png");

			binary = linescan::erode(binary, 3);

			save(binary, "04_erode.png");

			auto line = linescan::calc_line(binary);

			save(line, binary.height(), "05_line.png");

// 			load(image, "simulation/real_ref.png");
// 
// 			linescan::normelize_to_uint8(image);
// 
// 			auto edge = linescan::edge_amplitude(image);
// 
// 			save(edge, "06_edge.png");

			std::vector< linescan::point< float > > line1;
			std::vector< linescan::point< float > > line2;
			std::size_t count = 15;
			for(std::size_t i = 0; i < line.size() - count; ++i){
				if(line[i] == 0 || line[i + count] == 0) continue;

				if(line[i] > line[i + count]){
					line1.emplace_back(i, line[i]);
					continue;
				}

				for(i += count; i < line.size(); ++i){
					if(line[i] == 0) continue;
					line2.emplace_back(i, line[i]);
				}
			}

			std::cout << line1.size() << std::endl;
			std::cout << line2.size() << std::endl;

			if(line1.size() > count * 2){
				line1.erase(line1.begin(), line1.begin() + count);
				line1.erase(line1.end() - count, line1.end());
			}else{
				throw std::logic_error(
					"To less points in left laser line part"
				);
			}

			if(line2.size() > count * 2){
				line2.erase(line2.begin(), line2.begin() + count);
				line2.erase(line2.end() - count, line2.end());
			}else{
				throw std::logic_error(
					"To less points in right laser line part"
				);
			}

			{
				linescan::bitmap< std::uint8_t > lines(binary.width(), binary.height());
				draw(lines, line1);
				draw(lines, line2);
				save(lines, "07_lines.png");
			}

			auto l1 = linescan::fit_linear_function< float >(line1.begin(), line1.end());
			auto l2 = linescan::fit_linear_function< float >(line2.begin(), line2.end());

			{
				linescan::bitmap< std::uint8_t > lines(binary.width(), binary.height());
				draw(lines, l1);
				draw(lines, l2);
				save(lines, "08_lines.png");
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
