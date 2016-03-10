//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_central_image__hpp_INCLUDED_
#define _linescan__widget_central_image__hpp_INCLUDED_

#include <mitrax/matrix.hpp>

#include <QtWidgets/QtWidgets>


namespace linescan{


	class widget_central_image: public QMainWindow{
	public:
		void set_images(QImage const& image, QImage const& overlay);

		void set_overlay(QImage const& overlay);

		QImage image()const{ return image_; }

		QImage overlay()const{ return overlay_; }


	protected:
		virtual void paintEvent(QPaintEvent* event)override;


	private:
		QImage image_;
		QImage overlay_;
	};


	template < typename T >
	class widget_central_bitmap: public widget_central_image{
	public:
		using widget_central_image::set_images;

		void set_images(
			mitrax::raw_bitmap< T >&& image,
			QImage const& overlay
		);

		mitrax::raw_bitmap< T > const& bitmap()const{
			return bitmap_;
		}


	private:
		mitrax::raw_bitmap< T > bitmap_;
	};


}


#endif
