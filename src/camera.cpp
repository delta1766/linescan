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


	void throw_on_error(int code, char const* name){
		using namespace std::literals;

		if(code == IS_SUCCESS) return;

		throw std::runtime_error(name + " failed: "s + [code]{
			switch(code){
				case IS_NO_SUCCESS:
					return "General error message";
				case IS_INVALID_CAMERA_HANDLE:
					return "Invalid camera handle. Most of the uEye SDK "
						"functions expect the camera handle as the first "
						"parameter.";
				case IS_IO_REQUEST_FAILED:
					return "An IO request from the uEye driver failed. "
						"Possibly the versions of the ueye_api.dll (API) and "
						"the driver file (ueye_usb.sys or ueye_eth.sys) do "
						"not match.";
				case IS_CANT_OPEN_DEVICE:
					return "An attempt to initialize or select the camera "
						"failed (no camera connected or initialization "
						"error).";
				case IS_CANT_OPEN_REGISTRY:
					return "Error opening a Windows registry key.";
				case IS_CANT_READ_REGISTRY:
					return "Error reading settings from the Windows registry.";
				case IS_NO_IMAGE_MEM_ALLOCATED:
					return "The driver could not allocate memory.";
				case IS_CANT_CLEANUP_MEMORY:
					return "The driver could not release the allocated "
						"memory.";
				case IS_CANT_COMMUNICATE_WITH_DRIVER:
					return "Communication with the driver failed because no "
						"driver has been loaded.";
				case IS_FUNCTION_NOT_SUPPORTED_YET:
					return "The function is not supported yet.";
				case IS_INVALID_IMAGE_SIZE:
					return "Invalid image size.";
				case IS_INVALID_CAPTURE_MODE:
					return "The function can not be executed in the current "
						"camera operating mode (free run, trigger or "
						"standby).";
				case IS_INVALID_MEMORY_POINTER:
					return "Invalid pointer or invalid memory ID.";
				case IS_FILE_WRITE_OPEN_ERROR:
					return "File cannot be opened for writing or reading.";
				case IS_FILE_READ_OPEN_ERROR:
					return "The file cannot be opened.";
				case IS_FILE_READ_INVALID_BMP_ID:
					return "The specified file is not a valid bitmap file.";
				case IS_FILE_READ_INVALID_BMP_SIZE:
					return "The bitmap size is not correct (bitmap too large).";
				case IS_NO_ACTIVE_IMG_MEM:
					return "No active image memory available. You must set "
						"the memory to active using the is_SetImageMem() "
						"function or create a sequence using the "
						"is_AddToSequence() function.";
				case IS_SEQUENCE_LIST_EMPTY:
					return "The sequence list is empty and cannot be deleted.";
				case IS_CANT_ADD_TO_SEQUENCE:
					return "The image memory is already included in the "
						"sequence and cannot be added again.";
				case IS_SEQUENCE_BUF_ALREADY_LOCKED:
					return "The memory could not be locked. The pointer to "
						"the buffer is invalid.";
				case IS_INVALID_DEVICE_ID:
					return "The device ID is invalid. Valid IDs start from 1 "
						"for USB cameras, and from 1001 for GigE cameras.";
				case IS_INVALID_BOARD_ID:
					return "The board ID is invalid. Valid IDs range from 1 "
						"through 255.";
				case IS_ALL_DEVICES_BUSY:
					return "All cameras are in use.";
				case IS_TIMED_OUT:
					return "A timeout occurred. An image capturing process "
						"could not be terminated within the allowable period.";
				case IS_NULL_POINTER:
					return "Invalid array.";
				case IS_INVALID_PARAMETER:
					return "One of the submitted parameters is outside the "
						"valid range or is not supported for this sensor or "
						"is not available in this mode.";
				case IS_OUT_OF_MEMORY:
					return "No memory could be allocated.";
				case IS_ACCESS_VIOLATION:
					return "An internal error has occured.";
				case IS_NO_USB20:
					return "The camera is connected to a port which does not "
						"support the USB 2.0 high-speed standard. Cameras "
						"without a memory board cannot be operated on a USB "
						"1.1 port.";
				case IS_CAPTURE_RUNNING:
					return "A capturing operation is in progress and must be "
						"terminated first.";
				case IS_IMAGE_NOT_PRESENT:
					return "The requested image is not available in the "
						"camera memory or is no longer valid.";
				case IS_TRIGGER_ACTIVATED:
					return "The function cannot be used because the camera is "
						"waiting for a trigger signal.";
				case IS_CRC_ERROR:
					return "A CRC error-correction problem occurred while "
						"reading the settings.";
				case IS_NOT_YET_RELEASED:
					return "This function has not been enabled yet in this "
						"version.";
				case IS_NOT_CALIBRATED:
					return "The camera does not contain any calibration data.";
				case IS_WAITING_FOR_KERNEL:
					return "The system is waiting for the kernel driver to "
						"respond.";
				case IS_NOT_SUPPORTED:
					return "The camera model used here does not support this "
						"function or setting.";
				case IS_TRIGGER_NOT_ACTIVATED:
					return "The function is not possible as trigger is "
						"disabled.";
				case IS_OPERATION_ABORTED:
					return "The dialog was canceled without a selection so "
						"that no file could be saved.";
				case IS_BAD_STRUCTURE_SIZE:
					return "An internal structure has an incorrect size.";
				case IS_INVALID_BUFFER_SIZE:
					return "The image memory has an inappropriate size to "
						"store the image in the desired format.";
				case IS_INVALID_PIXEL_CLOCK:
					return "This setting is not available for the currently "
						"set pixel clock frequency.";
				case IS_INVALID_EXPOSURE_TIME:
					return "This setting is not available for the currently "
						"set exposure time.";
				case IS_AUTO_EXPOSURE_RUNNING:
					return "This setting cannot be changed while automatic "
						"exposure time control is enabled.";
				case IS_CANNOT_CREATE_BB_SURF:
					return "The BackBuffer surface cannot be created.";
				case IS_CANNOT_CREATE_BB_MIX:
					return "The BackBuffer mix surface cannot be created.";
				case IS_BB_OVLMEM_NULL:
					return "The BackBuffer overlay memory cannot be locked.";
				case IS_CANNOT_CREATE_BB_OVL:
					return "The BackBuffer overlay memory cannot be created.";
				case IS_NOT_SUPP_IN_OVL_SURF_MODE:
					return "Not supported in BackBuffer Overlay mode.";
				case IS_INVALID_SURFACE:
					return "Back buffer surface invalid.";
				case IS_SURFACE_LOST:
					return "Back buffer surface not found.";
				case IS_RELEASE_BB_OVL_DC:
					return "Error releasing the overlay device context.";
				case IS_BB_TIMER_NOT_CREATED:
					return "The back buffer timer could not be created.";
				case IS_BB_OVL_NOT_EN:
					return "The back buffer overlay was not enabled.";
				case IS_ONLY_IN_BB_MODE:
					return "Only possible in BackBuffer mode.";
				case IS_INVALID_COLOR_FORMAT:
					return "Invalid color format.";
				case IS_INVALID_WB_BINNING_MODE:
					return "Mono binning/mono sub-sampling do not support "
						"automatic white balance.";
				case IS_INVALID_I2C_DEVICE_ADDRESS:
					return "Invalid I2C device address";
				case IS_COULD_NOT_CONVERT:
					return "The current image could not be processed.";
				case IS_TRANSFER_ERROR:
					return "Transfer error. Frequent transfer errors can "
						"mostly be avoided by reducing the pixel rate.";
				case IS_PARAMETER_SET_NOT_PRESENT:
					return "Parameter set is not present.";
				case IS_INVALID_CAMERA_TYPE:
					return "The camera type defined in the .ini file does not "
						"match the current camera model.";
				case IS_INVALID_HOST_IP_HIBYTE:
					return "Invalid HIBYTE of host address.";
				case IS_CM_NOT_SUPP_IN_CURR_DISPLAYMODE:
					return "The color mode is not supported in the current "
						"display mode.";
				case IS_NO_IR_FILTER:
					return "No IR filter available";
				case IS_STARTER_FW_UPLOAD_NEEDED:
					return "The camera's starter firmware is not compatible "
						"with the driver and needs to be updated.";
				case IS_DR_LIBRARY_NOT_FOUND:
					return "The DirectRenderer library could not be found.";
				case IS_DR_DEVICE_OUT_OF_MEMORY:
					return "Not enough graphics memory available.";
				case IS_DR_CANNOT_CREATE_SURFACE:
					return "The image surface or overlay surface could not be "
						"created.";
				case IS_DR_CANNOT_CREATE_VERTEX_BUFFER:
					return "The vertex buffer could not be created.";
				case IS_DR_CANNOT_CREATE_TEXTURE:
					return "The texture could not be created.";
				case IS_DR_CANNOT_LOCK_OVERLAY_SURFACE:
					return "The overlay surface could not be locked.";
				case IS_DR_CANNOT_UNLOCK_OVERLAY_SURFACE:
					return "The overlay surface could not be unlocked.";
				case IS_DR_CANNOT_GET_OVERLAY_DC:
					return "Could not get the device context handle for the "
						"overlay.";
				case IS_DR_CANNOT_RELEASE_OVERLAY_DC:
					return "Could not release the device context handle for "
						"the overlay.";
				case IS_DR_DEVICE_CAPS_INSUFFICIENT:
					return "Function is not supported by the graphics "
						"hardware.";
				case IS_INCOMPATIBLE_SETTING:
					return "Because of other incompatible settings the "
						"function is not possible.";
				case IS_DR_NOT_ALLOWED_WHILE_DC_IS_ACTIVE:
					return "A device context handle is still open in the "
						"application.";
				case IS_DEVICE_ALREADY_PAIRED:
					return "The device is already paired.";
				case IS_SUBNETMASK_MISMATCH:
					return "The subnet mask of the camera and PC network card "
						"are different.";
				case IS_SUBNET_MISMATCH:
					return "The subnet of the camera and PC network card are "
						"different.";
				case IS_INVALID_IP_CONFIGURATION:
					return "The configuration of the IP address is invalid.";
				case IS_DEVICE_NOT_COMPATIBLE:
					return "The device is not compatible to the drivers.";
				case IS_NETWORK_FRAME_SIZE_INCOMPATIBLE:
					return "The settings for the image size of the camera are "
						"not compatible to the PC network card.";
				case IS_NETWORK_CONFIGURATION_INVALID:
					return "The configuration of the network card is invalid.";
				case IS_ERROR_CPU_IDLE_STATES_CONFIGURATION:
					return "The configuration of the CPU idle has failed.";
				case IS_DEVICE_BUSY:
					return "The camera is busy ad cannot transfer the "
						"requested image.";
				case IS_SENSOR_INITIALIZATION_FAILED:
					return "The initialization of the sensor failed.";
				default: return "unknown error";
			}
		}() + " (" + std::to_string(code) + ")");
	}

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

		throw_on_error(init, "is_InitCamera");

		SENSORINFO config;
		throw_on_error(is_GetSensorInfo(handle_, &config), "is_GetSensorInfo");

		if(config.nColorMode != IS_COLORMODE_MONOCHROME){
			throw std::runtime_error(
				"Camera is not monochrome!"
			);
		}

		throw_on_error(
			is_SetColorMode(handle_, IS_CM_MONO8), "is_SetColorMode"
		);

		width_ = config.nMaxWidth;
		height_ = config.nMaxHeight;
		std::cout << "cam width:  " << width_ << std::endl;
		std::cout << "cam height: " << height_ << std::endl;

		pixel_size_in_um_ = config.wPixelSize / 100.;

		throw_on_error(is_Exposure(
			handle_,
			IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN,
			&exposure_in_ms_min_,
			sizeof(exposure_in_ms_min_)
		), "is_Exposure(IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN)");

		throw_on_error(is_Exposure(
			handle_,
			IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX,
			&exposure_in_ms_max_,
			sizeof(exposure_in_ms_max_)
		), "is_Exposure(IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX)");

		throw_on_error(is_Exposure(
			handle_,
			IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_INC,
			&exposure_in_ms_inc_,
			sizeof(exposure_in_ms_inc_)
		), "is_Exposure(IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_INC)");
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


	double camera::pixel_size_in_um()const{
		return pixel_size_in_um_;
	}

	std::uint32_t camera::width()const{
		return width_;
	}

	std::uint32_t camera::height()const{
		return height_;
	}

	double camera::exposure_in_ms_min()const{
		return exposure_in_ms_min_;
	}

	double camera::exposure_in_ms_max()const{
		return exposure_in_ms_max_;
	}

	double camera::exposure_in_ms_inc()const{
		return exposure_in_ms_inc_;
	}

	double camera::exposure_in_ms()const{
		double exposure_in_ms;

		throw_on_error(is_Exposure(
			handle_,
			IS_EXPOSURE_CMD_GET_EXPOSURE,
			&exposure_in_ms,
			sizeof(exposure_in_ms)
		), "is_Exposure(IS_EXPOSURE_CMD_GET_EXPOSURE)");

		return exposure_in_ms;
	}


	void camera::set_exposure(double time_in_ms){
		throw_on_error(is_Exposure(
			handle_,
			IS_EXPOSURE_CMD_SET_EXPOSURE,
			&time_in_ms,
			sizeof(time_in_ms)
		), "is_Exposure(IS_EXPOSURE_CMD_SET_EXPOSURE)");
	}


	bitmap< std::uint8_t > camera::image(){
		using namespace std::literals;

		int mem_id = 0;
		char* buffer = nullptr;

		throw_on_error(
			is_AllocImageMem(handle_, width_, height_, 8, &buffer, &mem_id),
			"is_AllocImageMem"
		);

		throw_on_error(
			is_SetImageMem(handle_, buffer, mem_id),
			"is_SetImageMem"
		);

		throw_on_error(is_FreezeVideo(handle_, IS_WAIT), "is_FreezeVideo");

		bitmap< std::uint8_t > result(width_, height_);
		std::copy(buffer, buffer + width_ * height_, result.data());

		throw_on_error(
			is_FreeImageMem(handle_, buffer, mem_id),
			"is_FreeImageMem"
		);

		return result;
	}


}