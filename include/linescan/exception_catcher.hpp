//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__exception_catcher__hpp_INCLUDED_
#define _linescan__exception_catcher__hpp_INCLUDED_

#include <exception>
#include <iostream>

#include <QMessageBox>


namespace linescan{


	/// \brief Excecutes a function and catch all exceptions, log the exception
	///        via message box or command line
	///
	/// return true if not exception appeared
	template < typename F >
	bool exception_catcher(F&& f, bool make_message_box = true)try{
		f();
		return true;
	}catch(std::exception const& error){
		if(!make_message_box){
			std::cerr << "Exception: " << error.what() << std::endl;
			return false;
		}

		QMessageBox box(
			QMessageBox::Warning,
			QObject::tr("Error"),
			error.what(),
			QMessageBox::Ok
		);

		box.exec();

		return false;
	}catch(...){
		if(!make_message_box){
			std::cerr << "Unknown exception" << std::endl;
			return false;
		}

		QMessageBox box(
			QMessageBox::Critical,
			QObject::tr("Fatal Error"),
			"Unknown exception",
			QMessageBox::Ok
		);

		box.exec();

		return false;
	}


}


#endif
