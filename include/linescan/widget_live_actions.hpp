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

#include "widget_processing_base.hpp"


namespace linescan{


	class widget_live_actions: public widget_processing_base{
	public:
		using message_callback_type = std::function< void(QString const&) >;

		widget_live_actions(camera& cam, message_callback_type const& message);


	private:
		bool is_subpixel()const;

		message_callback_type message_;

		std::size_t save_count_;

		QRadioButton line_;

		QLabel subpixel_l_;
		QCheckBox subpixel_;
		QPushButton save_;
	};


}


#endif
