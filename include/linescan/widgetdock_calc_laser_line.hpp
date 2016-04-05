//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__widgetdock_calc_laser_line__hpp_INCLUDED_
#define _linescan__widgetdock_calc_laser_line__hpp_INCLUDED_

#include <QtWidgets/QtWidgets>


namespace linescan{


	class widgetdock_calc_laser_line: public QDockWidget{
	public:
		widgetdock_calc_laser_line();


	private:
		void show_box(QGroupBox& box);

		void update_threshold()const;
		void update_sum()const;


		QWidget main_widget_;
		QVBoxLayout main_layout_;


		// Main box with line calc method switch
		QGroupBox method_;
		QVBoxLayout method_layout_;
		QRadioButton method_threshold_;
		QRadioButton method_sum_;


		// Settings for calculation via threshold
		QGroupBox threshold_;
		QGridLayout threshold_layout_;

		QRadioButton threshold_show_original_;

		QRadioButton threshold_show_binarize_;
		QLabel threshold_binarize_l_;
		QSpinBox threshold_binarize_;

		QRadioButton threshold_show_erode_;
		QLabel threshold_erode_l_;
		QSpinBox threshold_erode_;

		QRadioButton threshold_show_line_;

		QLabel threshold_subpixel_l_;
		QCheckBox threshold_subpixel_;


		// Settings for calculation via sum
		QGroupBox sum_;
		QGridLayout sum_layout_;

		QRadioButton sum_show_original_;

		QLabel sum_min_value_l_;
		QSpinBox sum_min_value_;

		QLabel sum_min_sum_l_;
		QSpinBox sum_min_sum_;

		QRadioButton sum_show_line_;

		QLabel sum_subpixel_l_;
		QCheckBox sum_subpixel_;
	};


}


#endif
