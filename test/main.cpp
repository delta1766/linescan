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
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <iomanip>


int main()try{
	using namespace linescan;

	std::atomic< std::int64_t > preselection_x(0);
	std::atomic< std::int64_t > preselection_y(0);
	std::atomic< std::int64_t > preselection_z(0);
	std::atomic< std::int64_t > absolute_position_x(0);
	std::atomic< std::int64_t > absolute_position_y(0);
	std::atomic< std::int64_t > absolute_position_z(0);
	std::atomic< std::int64_t > status(0);
	std::atomic< char > command('c');
	std::atomic< std::int64_t > ramp(50);
	std::atomic< std::int64_t > motor_speed(50);
	std::atomic< std::int64_t > current_reduction(5);
	std::atomic< std::int64_t > mask(7);
	std::atomic< std::int64_t > delay_time_for_replies(2);
	std::atomic< bool > activate_cts(0);
	std::atomic< std::int64_t > leadscrew_pitch_x(40'000);
	std::atomic< std::int64_t > leadscrew_pitch_y(40'000);
	std::atomic< std::int64_t > leadscrew_pitch_z(40'000);
	std::atomic< std::int64_t > resolution(10);

	std::int64_t min_x = 0;
	std::int64_t min_y = 0;
	std::int64_t min_z = 0;
	std::int64_t max_x = 1'000'000'000;
	std::int64_t max_y = 1'000'000'000;
	std::int64_t max_z = 1'000'000'000;

	std::atomic< std::int64_t > x(0);
	std::atomic< std::int64_t > y(0);
	std::atomic< std::int64_t > z(0);
	std::atomic< std::int64_t > show_add_x(0);
	std::atomic< std::int64_t > show_add_y(0);
	std::atomic< std::int64_t > show_add_z(0);

	std::mutex mutex;
    auto start = std::chrono::high_resolution_clock::now();
	auto const calc = [&](auto action){
		std::lock_guard< std::mutex > lock(mutex);

		auto end = std::chrono::high_resolution_clock::now();
		auto distance = end - start;

		auto speed_x = leadscrew_pitch_x * motor_speed;
		auto speed_y = leadscrew_pitch_y * motor_speed;
		auto speed_z = leadscrew_pitch_z * motor_speed;

		switch(command){
			case 'a': break;
			case 'c':{
				
			} break;
			case 'e': break;
			case 'g': break;
			case 'j': break;
			case 'l': break;
			case 'm': break;
			case 'p': break;
			case 'r': break;
			case 's': break;
			case 'v': break;
		}

		action();

		start = end;

		std::cout <<
			std::setw(20) << x + show_add_x << ";" <<
			std::setw(20) << y + show_add_y << ";" <<
			std::setw(20) << z + show_add_z << std::endl;
	};

	bool run = true;
	auto worker = std::async([&run, &calc]{
		using namespace std::literals;

		while(run){
			calc([]{});

			std::this_thread::sleep_for(50ms);
		}
	});

	struct on_exit{
		on_exit(bool& run): run(run) {}

		~on_exit(){
			run = false;
		}

		bool& run;
	};


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
			case 0:{
				try{
					preselection_x = boost::lexical_cast< std::int64_t >(data);
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "preselection_x conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 1:{
				try{
					preselection_y = boost::lexical_cast< std::int64_t >(data);
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "preselection_y conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 2:{
				try{
					preselection_z = boost::lexical_cast< std::int64_t >(data);
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "preselection_z conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 3:{
				try{
					absolute_position_x = boost::lexical_cast< std::int64_t >(data);
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "absolute_position_x conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 4:{
				try{
					absolute_position_y = boost::lexical_cast< std::int64_t >(data);
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "absolute_position_y conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 5:{
				try{
					absolute_position_z = boost::lexical_cast< std::int64_t >(data);
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "absolute_position_z conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 6:{
				// Can be:
				//  'OK...'
				//  regex '^[AD@]{3}\\-\\.$'
			}break;
			case 7:{
				if(data.size() != 1 || std::string("acegjlmprsv").find(data[0]) == std::string::npos){
					std::cerr << "command unknown (value is '" << data << "')" << std::endl;
					break;
				}
				command = data[0];
			}break;
			case 8:{
				try{
					auto tmp = boost::lexical_cast< std::int64_t >(data);
					if(tmp < 1 || tmp > 99){
						std::cerr << "ramp out of range (1 .. 99, value is " << data << ")" << std::endl;
						break;
					}
					ramp = tmp;
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "ramp conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 9:{
				try{
					auto tmp = boost::lexical_cast< std::int64_t >(data);
					if(tmp < 0 || tmp > 150){
						std::cerr << "motor_speed out of range (0 .. 150, value is " << data << ")" << std::endl;
						break;
					}
					motor_speed = tmp;
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "motor_speed conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 10:{
				try{
					auto tmp = boost::lexical_cast< std::int64_t >(data);
					if(tmp < 0 || tmp > 10){
						std::cerr << "current_reduction out of range (0 .. 10, value is " << data << ")" << std::endl;
						break;
					}
					current_reduction = tmp;
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "current_reduction conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 11:{
				try{
					auto tmp = boost::lexical_cast< std::int64_t >(data);
					if(tmp < 0 || tmp > 7){
						std::cerr << "mask out of range (0 .. 7, value is " << data << ")" << std::endl;
						break;
					}
					mask = tmp;
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "mask conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 12:{
				try{
					auto tmp = boost::lexical_cast< std::int64_t >(data);
					if(tmp < 0 || tmp > 9){
						std::cerr << "delay_time_for_replies out of range (0 .. 9, value is " << data << ")" << std::endl;
						break;
					}
					delay_time_for_replies = tmp;
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "delay_time_for_replies conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 17:{
				if(data != "0" && data != "1"){
					std::cerr << "activate_cts out of range (0 .. 1, value is " << data << ")" << std::endl;
					break;
				}
				activate_cts = (data == "0" ? false : true);
			}break;
			case 21:{
				try{
					auto tmp = boost::lexical_cast< std::int64_t >(data);
					if(tmp < 1000 || tmp > 100000){
						std::cerr << "leadscrew_pitch_x out of range (1000 .. 100000, value is " << data << ")" << std::endl;
						break;
					}
					leadscrew_pitch_x = tmp;
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "leadscrew_pitch_x conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 22:{
				try{
					auto tmp = boost::lexical_cast< std::int64_t >(data);
					if(tmp < 1000 || tmp > 100000){
						std::cerr << "leadscrew_pitch_y out of range (1000 .. 100000, value is " << data << ")" << std::endl;
						break;
					}
					leadscrew_pitch_y = tmp;
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "leadscrew_pitch_y conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 23:{
				try{
					auto tmp = boost::lexical_cast< std::int64_t >(data);
					if(tmp < 1000 || tmp > 100000){
						std::cerr << "leadscrew_pitch_z out of range (1000 .. 100000, value is " << data << ")" << std::endl;
						break;
					}
					leadscrew_pitch_z = tmp;
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "leadscrew_pitch_z conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			case 25:{
				try{
					auto tmp = boost::lexical_cast< std::int64_t >(data);
					if(tmp < 1 || tmp > 10000){
						std::cerr << "resolution out of range (1 .. 10000, value is " << data << ")" << std::endl;
						break;
					}
					resolution = tmp;
				}catch(boost::bad_lexical_cast const& error){
					std::cerr << "resolution conversion error (value is '" << data << "')" << std::endl;
				}
			}break;
			// write commands
			case 64:{
				
			}break;
			case 65:{
				
			}break;
			case 66:{
				
			}break;
			case 67:{
				
			}break;
			case 68:{
				
			}break;
			case 69:{
				
			}break;
			case 70:{
				
			}break;
			case 71:{
				
			}break;
			case 72:{
				
			}break;
			case 73:{
				
			}break;
			case 74:{
				
			}break;
			case 75:{
				
			}break;
			case 76:{
				
			}break;
			case 80:{
				
			}break;
			case 81:{
				
			}break;
			case 85:{
				
			}break;
			case 86:{
				
			}break;
			case 87:{
				
			}break;
			case 89:{
				
			}break;
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