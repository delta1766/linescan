//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__control_F9S_base__hpp_INCLUDED_
#define _linescan__control_F9S_base__hpp_INCLUDED_

#include "serial_port.hpp"
#include "mask_non_print.hpp"
#include "vector.hpp"

#include <condition_variable>
#include <mutex>

#ifdef HARDWARE
#ifndef MCL
#define MCL
#endif
#ifndef CAM
#define CAM
#endif
#endif

#if defined(MCL) && defined(CAM) && !defined(HARDWARE)
#define HARDWARE
#endif


namespace linescan{


	class control_F9S_base{
	public:
		control_F9S_base(std::string const& device);


	protected:
		struct command{
			command(std::uint8_t address, std::string&& data = ""):
				address(address), data(std::move(data)) {}

			command(std::uint8_t address, char data):
				address(address), data(1, data) {}

			command(std::uint8_t address, std::int64_t data):
				address(address), data(std::to_string(data)) {}

			std::uint8_t address;
			std::string data;
		};


		virtual std::string name()const = 0;

		void send(vector< command > const& commands);
		void send(char const* data);

		std::string get(
			vector< command > const& commands,
			std::size_t repetitions = 3
		);

		template < typename Rep, typename Period >
		std::string get(
			vector< command > const& commands,
			std::chrono::duration< Rep, Period > const& timeout,
			std::size_t repetitions = 3
		){
			for(std::size_t i = 0; i < repetitions; ++i){
				send(commands);
				auto result = receive(timeout);
				if(result.second) return result.first;
			}

			throw std::runtime_error("no answer from " + name());
		}

		std::pair< std::string, bool > receive();

		template < typename Rep, typename Period >
		std::pair< std::string, bool > receive(
			std::chrono::duration< Rep, Period > const& timeout
		){
			std::unique_lock< std::mutex > lock(mutex_);
			bool ok =
				cv_.wait_for(lock, timeout) == std::cv_status::no_timeout;
			return {receive_, ok};
		}

		/// \brief sleep for 50ms
		///
		/// After write commands without answer you should wait a few ms.
		void delay()const;


	private:
		std::mutex mutex_;
		std::condition_variable cv_;

		std::string receive_;

#ifdef MCL
		serial_port port_;
#endif
	};


}


#endif
