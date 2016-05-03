//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__control_F9S_base__hpp_INCLUDED_
#define _linescan__control_F9S_base__hpp_INCLUDED_

#include "serial_port.hpp"

#include <condition_variable>
#include <vector>
#include <mutex>

#ifndef EMULATION
#ifndef MCL
#define MCL
#endif
#ifndef CAM
#define CAM
#endif
#endif


namespace linescan{


	/// \brief Base class for communication with a MCL box
	class control_F9S_base{
	public:
		/// \brief Open a serial port which is connected with a MCL box
		control_F9S_base(std::string const& device);


	protected:
		/// \brief Command for a MCL box
		struct command{
			/// \brief Command with text data or without data
			command(std::uint8_t address, std::string&& data = ""):
				address(address), data(std::move(data)) {}

			/// \brief Command a single char as data
			command(std::uint8_t address, char data):
				address(address), data(1, data) {}

			/// \brief Command with an integer as data
			command(std::uint8_t address, std::int64_t data):
				address(address), data(std::to_string(data)) {}


			/// \brief Command as char
			std::uint8_t address;

			/// \brief Text data of the command
			std::string data;
		};


		/// \brief Name of the MCL box for error messages
		virtual std::string name()const = 0;


		/// \brief Send commands to MCL
		void send(std::vector< command > const& commands);

		/// \brief Send raw data to MCL
		void send(char const* data);


		/// \brief Send commands and wait for result from MCL
		///
		/// If the box does not answer after 200ms, the send is repeated.
		std::string get(
			std::vector< command > const& commands,
			std::size_t repetitions = 3
		);

		/// \brief Send commands and wait for result from MCL
		///
		/// If the box does not answer after timeout, the send is repeated.
		template < typename Rep, typename Period >
		std::string get(
			std::vector< command > const& commands,
			std::chrono::duration< Rep, Period > const& timeout,
			std::size_t repetitions = 3
		){
#ifdef MCL
			for(std::size_t i = 0; i < repetitions; ++i){
				send(commands);
				auto result = receive(timeout);
				if(result.second) return result.first;
			}

			throw std::runtime_error("no answer from " + name());
#else
			throw std::logic_error(
				"call control_F9S_base::get in simulation mode"
			);
			(void)commands;
			(void)timeout;
			(void)repetitions;
#endif
		}

		/// \brief Get answer from MCL
		///
		/// Wait maximal 200ms for an answer.
		std::pair< std::string, bool > receive();

		/// \brief Get answer from MCL
		///
		/// Wait maximal timeout for an answer.
		template < typename Rep, typename Period >
		std::pair< std::string, bool > receive(
			std::chrono::duration< Rep, Period > const& timeout
		){
#ifdef MCL
			std::unique_lock< std::mutex > lock(mutex_);
			bool ok =
				cv_.wait_for(lock, timeout) == std::cv_status::no_timeout;
			return {receive_, ok};
#else
			throw std::logic_error(
				"call control_F9S_base::receive in simulation mode"
			);
			(void)timeout;
#endif
		}

		/// \brief sleep for 50ms
		///
		/// After write commands without answer you should wait a few ms.
		void delay()const;


	private:
#ifdef MCL
		std::mutex mutex_;
		std::condition_variable cv_;
#endif

		std::string receive_;

#ifdef MCL
		serial_port port_;
#endif
	};


}


#endif
