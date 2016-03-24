//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
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

#include <QtWidgets/QMessageBox>


namespace linescan{


	template < typename F >
	void exception_catcher(F&& f, bool make_message = true)try{
		f();
	}catch(std::exception const& error){
		if(!make_message){
			std::cerr << "Exception: " << error.what() << std::endl;
			return;
		}

		QMessageBox box(
			QMessageBox::Warning,
			QObject::tr("Error"),
			error.what(),
			QMessageBox::Ok
		);

		box.exec();
	}catch(...){
		if(!make_message){
			std::cerr << "Unknown exception" << std::endl;
			return;
		}

		QMessageBox box(
			QMessageBox::Critical,
			QObject::tr("Fatal Error"),
			"Unknown exception",
			QMessageBox::Ok
		);

		box.exec();
	}


}


#endif
