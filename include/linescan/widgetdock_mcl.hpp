//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widgetdock_mcl__hpp_INCLUDED_
#define _linescan__widgetdock_mcl__hpp_INCLUDED_

#include "control_F9S_MCL3.hpp"

#include <QtWidgets>


namespace linescan{


	/// \brief Widget for MCL settings
	class widgetdock_mcl: public QDockWidget{
	public:
		/// \brief Constructor
		widgetdock_mcl(control_F9S_MCL3& mcl3);


	private:
		control_F9S_MCL3& mcl3_;

		QWidget widget_;
		QVBoxLayout layout_;

		QLabel x_;
		QLabel y_;
		QLabel z_;

		QPushButton joystick_;
		QPushButton reset_pos_;

		QTimer timer_;
	};


}


#endif
