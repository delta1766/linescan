//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/camera.hpp>

#include <ueye.h>
#include <sys/mman.h>
#include <errno.h>

#include <iostream>
#include <thread>


namespace linescan{


	camera::camera(std::uint32_t cam_id):
		handle_(cam_id)
	{
		using namespace std::literals;

		int init;

		// try 3 times
		for(std::size_t i = 0; i < 3; ++i){
			init = is_InitCamera(&handle_, nullptr);
			std::this_thread::sleep_for(500ms);
			if(init == IS_SUCCESS) break;
		}

		switch(init){
			case IS_SUCCESS: break;
			default:
				throw std::runtime_error(
					"is_InitCamera failed: " + std::to_string(init)
				);
		}

		SENSORINFO config;
		auto info = is_GetSensorInfo(handle_, &config);
		switch(info){
			case IS_SUCCESS: break;
			default:
				throw std::runtime_error(
					"is_GetSensorInfo failed: " + std::to_string(info)
				);
		}

		if(config.nColorMode != IS_COLORMODE_MONOCHROME){
			throw std::runtime_error(
				"Camera is not monochrome!"
			);
		}

		auto color_mode = is_SetColorMode(handle_, IS_CM_MONO8);
		switch(color_mode){
			case IS_SUCCESS: break;
			default:
				throw std::runtime_error(
					"is_SetColorMode failed: " + std::to_string(color_mode)
				);
		}

		width_ = config.nMaxWidth;
		height_ = config.nMaxHeight;
		std::cout << "cam width:  " << width_ << std::endl;
		std::cout << "cam height: " << height_ << std::endl;

		pixel_size_in_um_ = config.wPixelSize / 100.;
	}


	camera::~camera()try{
		close();
	}catch(std::runtime_error const& error){
		std::cerr <<
			"Exception in camera destructor, please use close() explicit (" <<
			error.what() << ")" << std::endl;
	}catch(...){
		std::cerr <<
			"Unknown exception in camera destructor, please use close() "
			"explicit" << std::endl;
	}


	void camera::close(){
		auto exit = is_ExitCamera(handle_);
		switch(exit){
			case IS_SUCCESS: break;
			default:
				throw std::runtime_error(
					"is_ExitCamera failed: " + std::to_string(exit)
				);
		}
	}


	bitmap< std::uint8_t > camera::image(){
		using namespace std::literals;

		int mem_id = 0;
		char* buffer = nullptr;

		auto alloc = is_AllocImageMem(handle_, width_, height_, 8, &buffer, &mem_id);
		switch(alloc){
			case IS_SUCCESS: break;
			default:
				throw std::runtime_error(
					"is_AllocImageMem failed: " + std::to_string(alloc)
				);
		}

		auto set = is_SetImageMem(handle_, buffer, mem_id);
		switch(set){
			case IS_SUCCESS: break;
			default:
				throw std::runtime_error(
					"is_SetImageMem failed: " + std::to_string(set)
				);
		}

		auto get_image = is_FreezeVideo(handle_, IS_WAIT);
		switch(get_image){
			case IS_SUCCESS: break;
			default:
				throw std::runtime_error(
					"is_FreezeVideo failed: " + std::to_string(get_image)
				);
		}

		bitmap< std::uint8_t > result(width_, height_);
		std::copy(buffer, buffer + width_ * height_, result.data());

		auto free = is_FreeImageMem(handle_, buffer, mem_id);
		switch(free){
			case IS_SUCCESS: break;
			default:
				throw std::runtime_error(
					"is_FreeImageMem failed: " + std::to_string(free)
				);
		}

		return result;
	}


}
