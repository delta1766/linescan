//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_main_window.hpp>

#include <boost/type_index.hpp>

#include <iostream>


int main(int argc, char** argv){
	QApplication app(argc, argv);

	QTranslator translator;
	if(translator.load(QString("lang/linescan_de.qm"))){
		std::cout << "success" << std::endl;
		app.installTranslator(&translator);
	}

	try{
		QCoreApplication::setOrganizationName("TU Ilmenau");
		QCoreApplication::setOrganizationDomain("tu-ilmenau.de");
		QCoreApplication::setApplicationName("linescan");

		linescan::widget_main_window window;
		window.showMaximized();

		return app.exec();
	}catch(std::exception const& e){
		QMessageBox box(
			QMessageBox::Critical,
			QObject::tr("Error"),
			QObject::tr("Exit with exception: [%1] %2")
				.arg(boost::typeindex::type_id_runtime(e).pretty_name().c_str())
				.arg(e.what()),
			QMessageBox::Ok
		);

		box.exec();
	}catch(...){
		QMessageBox box(
			QMessageBox::Critical,
			QObject::tr("Fatal Error"),
			QObject::tr("Exit with unknown exception!"),
			QMessageBox::Ok
		);

		box.exec();
	}

	return 1;
}
