//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_central_image__hpp_INCLUDED_
#define _linescan__widget_central_image__hpp_INCLUDED_

#include <QtWidgets>


namespace linescan{


	/// \brief Show an image and optional an overlay
	class widget_central_image: public QMainWindow{
	public:
		/// \brief Set image and overlay
		void set_images(QImage const& image, QImage const& overlay);

		/// \brief Set overlay
		void set_overlay(QImage const& overlay);


		/// \brief Get image
		QImage image()const{ return image_; }

		/// \brief Get overlay
		QImage overlay()const{ return overlay_; }


	protected:
		/// \brief Scale and paint image and its overlay
		virtual void paintEvent(QPaintEvent* event)override;


	private:
		QImage image_;
		QImage overlay_;
	};


}


#endif
