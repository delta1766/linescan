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


#include <linescan/circlefind.hpp>
#include <linescan/load.hpp>


int main(int argc, char** argv)try{
// 	{
		auto m = linescan::load("data/live_0000.png");
		linescan::circlefind(m, 12, 9, 1, 2.5);
// 	} std::cin.get();
// 	{
// 		auto m = linescan::load("data/live_0001.png");
// 		linescan::circlefind(m, 12, 9, 1, 2.5);
// 	} std::cin.get();
// 	{
// 		auto m = linescan::load("data/live_0002.png");
// 		linescan::circlefind(m, 12, 9, 1, 2.5);
// 	} std::cin.get();
// 	{
// 		auto m = linescan::load("data/live_0003.png");
// 		linescan::circlefind(m, 12, 9, 1, 2.5);
// 	} std::cin.get();
// 	{
// 		auto m = linescan::load("data/live_0004.png");
// 		linescan::circlefind(m, 12, 9, 1, 2.5);
// 	} std::cin.get();
// 	{
// 		auto m = linescan::load("data/live_0005.png");
// 		linescan::circlefind(m, 12, 9, 1, 2.5);
// 	} std::cin.get();
// 	{
// 		auto m = linescan::load("data/live_0006.png");
// 		linescan::circlefind(m, 12, 9, 1, 2.5);
// 	} std::cin.get();
// 	{
// 		auto m = linescan::load("data/live_0007.png");
// 		linescan::circlefind(m, 12, 9, 1, 2.5);
// 	} std::cin.get();
// 	{
// 		auto m = linescan::load("data/live_0008.png");
// 		linescan::circlefind(m, 12, 9, 1, 2.5);
// 	}

	return 0;

	QApplication app(argc, argv);

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
