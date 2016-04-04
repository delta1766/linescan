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
	QCoreApplication::setApplicationName("linescan");
	QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

	linescan::widget_main_window window;
	window.showMaximized();

	return app.exec();
}catch(std::exception const& e){
	QApplication app(argc, argv);

	QMessageBox box(
		QMessageBox::Critical,
		QObject::tr("Crash"),
		QString("Exit with exception: [%1] %2")
		.arg(boost::typeindex::type_id_runtime(e).pretty_name().c_str())
		.arg(e.what()),
		QMessageBox::Ok
	);

	box.exec();
}catch(...){
	QApplication app(argc, argv);

	QMessageBox box(
		QMessageBox::Critical,
		QObject::tr("Fatal Crash"),
		"Exit with unknown exception!",
		QMessageBox::Ok
	);

	box.exec();
}
