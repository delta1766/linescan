//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_live_image__hpp_INCLUDED_
#define _linescan__widget_live_image__hpp_INCLUDED_

#include "widget_central_image.hpp"
#include "camera.hpp"


namespace linescan{


	/// \brief Live image widget
	class widget_live_image: public widget_central_image{
	public:
		/// \brief Camera image type
		using bitmap = mitrax::std_bitmap< std::uint8_t >;

		/// \brief Processor type without overlay
		using processor1_type = std::function< QImage(bitmap&&) >;

		/// \brief Processor type with overlay
		using processor2_type =
			std::function< std::pair< QImage, QImage >(bitmap&&) >;


		/// \brief Constructor
		widget_live_image(camera& cam);


		/// \brief Set image processor without overlay
		void set_processor(processor1_type const& function);

		/// \brief Set image processor with overlay
		void set_processor(processor2_type const& function);


		/// \brief Set an image processor that simply show's the camera image
		void reset_processor();


		/// \brief true, if live image calculation is enabled
		bool is_live()const;

		/// \brief Stop live image calculation
		///
		/// You can now use the central image functionality.
		void stop_live();

		/// \brief Start live image calculation
		///
		/// Start a loop that gets a camera image, processes it and shows the
		/// result.
		void start_live();


	protected:
		/// \brief Start live image
		virtual void showEvent(QShowEvent* event)override;

		/// \brief Stop live image
		virtual void hideEvent(QHideEvent* event)override;


	private:
		/// \brief Enable/disable live camera processing
		void set_live(bool is_live);

		camera& cam_;

		bool is_live_;

		processor2_type processor_;

		QTimer timer_;
	};


}


#endif
