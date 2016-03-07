//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widget_live_actions__hpp_INCLUDED_
#define _linescan__widget_live_actions__hpp_INCLUDED_

#include "widget_live_image.hpp"

#include <QtWidgets/QtWidgets>


namespace linescan{


	class widget_live_actions: public QWidget{
	public:
		using message_callback_type = std::function< void(QString const&) >;

		widget_live_actions(camera& cam, message_callback_type const& message);


	private:
		std::uint8_t get_threashold()const;
		std::uint8_t get_erode()const;
		bool is_sub_pixel()const;

		mitrax::raw_bitmap< bool > binarized(
			mitrax::raw_bitmap< std::uint8_t > const& bitmap
		)const;

		mitrax::raw_bitmap< bool > eroded(
			mitrax::raw_bitmap< std::uint8_t > const& bitmap
		)const;


		std::size_t save_count_;

		message_callback_type message_;

		QHBoxLayout hlayout_;
		QGridLayout glayout_;

		QRadioButton original_;
		QRadioButton binarized_;
		QRadioButton eroded_;
		QRadioButton line_;

		QLabel binarize_threashold_l_;
		QLabel erode_l_;
		QLabel sub_pixel_l_;
		QSpinBox binarize_threashold_;
		QSpinBox erode_;
		QCheckBox sub_pixel_;
		QPushButton save_;

		widget_live_image image_;
	};


}


#endif
