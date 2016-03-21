//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_calib_via_line.hpp>
#include <linescan/exception_catcher.hpp>
#include <linescan/draw.hpp>


namespace linescan{


	widget_calib_via_line::widget_calib_via_line(
		camera& cam,
		control_F9S_MCL3& mcl3
	):
		widget_processing_base(cam),
		mcl3_(mcl3),
		line_(tr("Line image")),
		start_(tr("Start")),
		running_(false)
	{
		glayout_.addWidget(&line_, 5, 0, 1, 2);
		glayout_.addWidget(&start_, 6, 0, 1, 2);
		glayout_.setRowStretch(7, 1);

		connect(&line_, &QRadioButton::released, [this]{
			if(!line_.isChecked()) return;

			image_.set_processor(&draw_laser_alignment);
		});

		connect(&start_, &QPushButton::released, [this]{
			timer_.start(10);
		});

		connect(&timer_, &QTimer::timeout, [this]{
			exception_catcher([&]{
				if(running_) timer_.start(10);
			});
		});

	}

	bool widget_calib_via_line::is_running()const{
		return running_;
	}

	void widget_calib_via_line::stop(){
		set_running(false);
		timer_.stop();
	}

	void widget_calib_via_line::start(){
		set_running(true);
		timer_.start(0);
	}

	void widget_calib_via_line::showEvent(QShowEvent* event){
		QWidget::showEvent(event);
		start();
	}

	void widget_calib_via_line::hideEvent(QHideEvent* event){
		QWidget::hideEvent(event);
		stop();
	}

	void widget_calib_via_line::set_running(bool is_running){
		running_ = is_running;
	}


}
