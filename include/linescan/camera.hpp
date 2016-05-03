//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
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
#include <array>

#ifndef EMULATION
#ifndef MCL
#define MCL
#endif
#ifndef CAM
#define CAM
#endif
#endif


namespace linescan{


	class camera{
	public:
		/// \brief Constructor with camera id
		///
		/// cam_id is 0, if there is only one camera
		explicit camera(std::uint32_t cam_id);

		/// \brief Destructor
		~camera();


		/// \brief Get a camera image
		mitrax::raw_bitmap< std::uint8_t > image();


		/// \brief Width of the camera in pixel
		std::uint32_t cols()const;

		/// \brief Height of the camera in pixel
		std::uint32_t rows()const;

		/// \brief Pixel size in µm
		double pixel_size_in_um()const;


		/// \brief Uninitialize the camera
		void close();


		/// \brief Min, max and step width of the pixelclock
		std::array< std::uint32_t, 3 > pixelclock_min_max_inc()const;

		/// \brief Min value of the pixelclock
		std::uint32_t pixelclock_min()const;

		/// \brief Max value of the pixelclock
		std::uint32_t pixelclock_max()const;

		/// \brief Step width of the pixelclock
		std::uint32_t pixelclock_inc()const;

		/// \brief Value of the pixelclock
		std::uint32_t pixelclock()const;

		/// \brief Set pixelclock value
		void set_pixelclock(std::uint32_t pixelclock);


		/// \brief Min, max and step width of the framerate
		std::array< double, 3 > framerate_min_max_inc()const;

		/// \brief Min value of the framerate
		double framerate_min()const;

		/// \brief Max value of the framerate
		double framerate_max()const;

		/// \brief Step width of the framerate
		double framerate_inc()const;

		/// \brief Value of the framerate
		double framerate()const;

		/// \brief Set framerate value
		void set_framerate(double framerate);


		/// \brief Min, max and step width of the exposure
		std::array< double, 3 > exposure_in_ms_min_max_inc()const;

		/// \brief Min value of the exposure
		double exposure_in_ms_min()const;

		/// \brief Max value of the exposure
		double exposure_in_ms_max()const;

		/// \brief Step width of the exposure
		double exposure_in_ms_inc()const;

		/// \brief Value of the exposure
		double exposure_in_ms()const;

		/// \brief Set exposure value
		void set_exposure(double time_in_ms);


		/// \brief Get gain in percent
		std::size_t gain_in_percent()const;

		/// \brief Set gain in percent
		void set_gain(std::size_t percent);

		/// \brief Set gain by camera calculation
		void set_gain_auto();


		/// \brief true if gain boost is enabled
		bool gain_boost()const;

		/// \brief enabled or disable gain boost
		void set_gain_boost(bool on);


		/// \brief Get pixelclock, framerate, exposure, gain and gain boost
		std::tuple< std::uint32_t, double, double, size_t, bool >
		get_light_params()const;

		/// \brief Set pixelclock, framerate, exposure, gain and gain boost
		void set_light_params(
			std::uint32_t pixelclock,
			double framerate,
			double exposure_in_ms,
			std::size_t gain_in_percent,
			bool gain_boost
		);


		/// \brief Set all parameters to a maximal bright image
		void set_max_light();

		/// \brief Set all parameters to a maximal dark image
		void set_min_light();

		/// \brief Set all parameters to the values after camera initialization
		void set_default_light();


	private:
#ifdef CAM
		std::uint32_t handle_;
#else
		size_t i_;
#endif

		std::uint32_t cols_;
		std::uint32_t rows_;

		double pixel_size_in_um_;

		std::uint32_t default_pixelclock_;
		double default_framerate_;
		double default_exposure_in_ms_;
		std::size_t default_gain_in_percent_;
		bool default_gain_boost_;
	};


}


#endif
