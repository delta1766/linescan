//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__camera__hpp_INCLUDED_
#define _linescan__camera__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <tuple>


namespace linescan{


	class camera{
	public:
		explicit camera(std::uint32_t cam_id);

		~camera();


		mitrax::raw_bitmap< std::uint8_t > image();


		std::uint32_t cols()const;
		std::uint32_t rows()const;
		double pixel_size_in_um()const;


		void close();

		std::uint32_t pixelclock_min()const;
		std::uint32_t pixelclock_max()const;
		std::uint32_t pixelclock_inc()const;
		std::uint32_t pixelclock()const;
		void set_pixelclock(std::uint32_t pixelclock);

		double framerate_min()const;
		double framerate_max()const;
		double framerate_inc()const;
		double framerate()const;
		void set_framerate(double framerate);

		double exposure_in_ms_min()const;
		double exposure_in_ms_max()const;
		double exposure_in_ms_inc()const;
		double exposure_in_ms()const;
		void set_exposure(double time_in_ms);

		std::size_t gain_in_percent()const;
		void set_gain(std::size_t percent);
		void set_gain_auto();

		bool gain_boost()const;
		void set_gain_boost(bool on);

		std::tuple< std::uint32_t, double, double, size_t, bool >
		get_light_params()const;

		void set_light_params(
			std::uint32_t pixelclock,
			double framerate,
			double exposure_in_ms,
			std::size_t gain_in_percent,
			bool gain_boost
		);

		void set_max_light();


	private:
#ifdef CAM
		std::uint32_t handle_;
#else
		size_t i_;
#endif

		std::uint32_t cols_;
		std::uint32_t rows_;

		double pixel_size_in_um_;
	};


}


#endif
