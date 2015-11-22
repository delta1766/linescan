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

		double exposure_in_ms_min()const;
		double exposure_in_ms_max()const;
		double exposure_in_ms_inc()const;
		double exposure_in_ms()const;

		void set_exposure(double time_in_ms);


	private:
		std::uint32_t handle_;

		std::uint32_t cols_;
		std::uint32_t rows_;

		double pixel_size_in_um_;

		double exposure_in_ms_min_;
		double exposure_in_ms_max_;
		double exposure_in_ms_inc_;
	};


}


#endif
