//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/serial_port.hpp>
#include <linescan/mask_non_print.hpp>

#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/type_index.hpp>

#include <algorithm>
#include <iostream>


namespace linescan{


	serial_port::serial_port(
		callback&& callback,
		bool separator_active,
		std::string&& separator
	):
		callback_(callback),
		port_(io_),
		separator_active_(separator_active),
		separator_(std::move(separator))
	{
		std::cout << "'" << mask_non_print(separator_) << "'" << std::endl;
	}

	serial_port::serial_port(callback&& callback, std::string&& separator):
		serial_port(std::move(callback), true, std::move(separator)){}


	serial_port::~serial_port()try{
		close();
	}catch(std::exception const& e){
		std::cerr
			<< "serial_port deconstructed with exception, use member "
			<< "function close() to catch exceptions: ["
			<< boost::typeindex::type_id_runtime(e).pretty_name() << "] "
			<< e.what() << std::endl;
	}catch(...){
		std::cerr
			<< "serial_port deconstructed with unknown exception, use member "
			<< "function close() to catch exceptions" << std::endl;
	}


	void serial_port::open(
		std::string const& device,
		unsigned baud_rate,
		unsigned character_size,
		flow_control::type fc,
		parity::type p,
		stop_bits::type sb
	)try{
		std::lock_guard< std::mutex > lock(mutex);

		if(port_.is_open()) close();

		port_.open(device);

		port_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
		port_.set_option(
			boost::asio::serial_port_base::character_size(character_size)
		);
		port_.set_option(flow_control(fc));
		port_.set_option(parity(p));
		port_.set_option(stop_bits(sb));

		future_ = std::async(std::launch::async, [this]{
			read();
			io_.run();
		});
	}catch(boost::system::system_error e){
		throw boost::system::system_error(
			e.code(),
			("serial_port (" + device + "): " + e.what()).c_str()
		);
	}

	void serial_port::close(){
		{
			std::lock_guard< std::mutex > lock(mutex);

			io_.stop();
			if(port_.is_open()) port_.close();
		}

		if(future_.valid()) future_.wait();
	}

	void serial_port::send(std::string const& text){
		std::lock_guard< std::mutex > lock(mutex);

		if(!port_.is_open()){
			throw std::runtime_error(
				"serial_port can not send '" + text + "', port is not open"
			);
		}

		boost::asio::write(
			port_,
			boost::asio::buffer(text.c_str(), text.size())
		);
	}

	void serial_port::read(){
		port_.async_read_some(
			boost::asio::buffer(recieved_.data(), recieved_.size()),
			[this](
				boost::system::error_code const& error,
				std::size_t bytes_transferred
			){
				if(error){
					if(error.value() == boost::asio::error::operation_aborted){
						return; // port_.close();
					}

					throw error;
				}

				if(separator_active_){
					std::copy(
						recieved_.begin(),
						recieved_.begin() + bytes_transferred,
						std::back_inserter(buffer_)
					);

					for(
						auto end = buffer_.find(separator_);
						end != std::string::npos;
						end = buffer_.find(separator_)
					){
						auto text = buffer_.substr(0, end);
						buffer_ = buffer_.substr(end + separator_.size());
						callback_(text);
					}
				}else{
					callback_(std::string(
						recieved_.begin(),
						recieved_.begin() + bytes_transferred
					));
				}

				read();
			}
		);
	}


}
