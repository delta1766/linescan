//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__serial_port__hpp_INCLUDED_
#define _linescan__serial_port__hpp_INCLUDED_

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>

#include <future>
#include <type_traits>
#include <functional>


namespace linescan{


	using parity = boost::asio::serial_port_base::parity;
	using stop_bits = boost::asio::serial_port_base::stop_bits;
	using flow_control = boost::asio::serial_port_base::flow_control;

	/// \brief Serial port manager
	class serial_port{
	public:
		/// \brief Callback function type
		using callback = std::function< void(std::string&&) >;


		/// \brief Constructor with simulation support
		///
		/// \param callback Is called on every single message receive
		/// \param separator_active If false it is in simulation mode
		/// \param separator Separator between receive messages
		serial_port(
			callback&& callback,
			bool separator_active,
			std::string&& separator
		);

		/// \brief Constructor
		///
		/// \param callback Is called on every single message receive
		/// \param separator Separator between receive messages
		serial_port(callback&& callback, std::string&& separator);

		/// \brief Destructor
		~serial_port();


		/// \brief Open connection
		void open(
			std::string const& device,
			unsigned baud_rate,
			unsigned character_size,
			flow_control::type flow_control,
			parity::type parity,
			stop_bits::type stop_bits
		);

		/// \brief Close connection
		void close();


		/// \brief Send text
		void send(std::string const& text);


	private:
		/// \brief Async read messages
		void read();

		callback const callback_;
		boost::asio::io_service io_;
		boost::asio::serial_port port_;

		std::array< char, 8192 > recieved_;
		std::string buffer_;

		bool const separator_active_;
		std::string const separator_;

		std::mutex mutex;

		/// \brief Async Task
		///
		/// Note: Last constructed, first deconstructed
		std::future< void > future_;
	};


}


#endif
