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

#include "bitmap.hpp"


namespace linescan{


	class camera{
	public:
		explicit camera(std::uint32_t cam_id);

		~camera();


		bitmap< std::uint8_t > image();


		std::uint32_t width()const;
		std::uint32_t height()const;
		double pixel_size_in_um()const;


		void close();

		double exposure_in_ms_min()const;
		double exposure_in_ms_max()const;
		double exposure_in_ms_inc()const;
		double exposure_in_ms()const;

	private:
		std::uint32_t handle_;

		std::uint32_t width_;
		std::uint32_t height_;

		double pixel_size_in_um_;

		double exposure_in_ms_min_;
		double exposure_in_ms_max_;
		double exposure_in_ms_inc_;
		double exposure_in_ms_;
	};


}


#endif
