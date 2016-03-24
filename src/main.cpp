//-----------------------------------------------------------------------------
// Copyright (c) 2015-2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_main_window.hpp>

#include <boost/type_index.hpp>

#include <iostream>


int main(int argc, char** argv)try{
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("TU Ilmenau");
	QCoreApplication::setOrganizationDomain("tu-ilmenau.de");
	QCoreApplication::setApplicationName("Linescan");
	QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

	linescan::widget_main_window window;
	window.showMaximized();

	return app.exec();
}catch(std::exception const& e){
	std::cerr
		<< "Exit with exception: ["
		<< boost::typeindex::type_id_runtime(e).pretty_name() << "] "
		<< e.what() << std::endl;
}catch(...){
	std::cerr << "Exit with unknown exception" << std::endl;
}
