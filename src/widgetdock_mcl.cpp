//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widgetdock_mcl.hpp>
#include <linescan/exception_catcher.hpp>
#include <linescan/block_signals.hpp>


namespace linescan{


	widgetdock_mcl::widgetdock_mcl(control_F9S_MCL3& mcl3):
		QDockWidget(tr("MCL")),
		mcl3_(mcl3),
		joystick_(tr("Joystick")),
		reset_pos_(tr("Reset position"))
	{
		layout_.addWidget(&x_);
		layout_.addWidget(&y_);
		layout_.addWidget(&z_);
		layout_.addWidget(&joystick_);
		layout_.addWidget(&reset_pos_);
		layout_.addStretch();

		widget_.setLayout(&layout_);
		setWidget(&widget_);


		joystick_.setCheckable(true);
		timer_.setSingleShot(true);


		auto set_pos = [this](std::int64_t x, std::int64_t y, std::int64_t z){
			x_.setText(tr("X: %1 mm").arg(x / 1000.));
			y_.setText(tr("Y: %1 mm").arg(y / 1000.));
			z_.setText(tr("Z: %1 mm").arg(z / 1000.));
		};

		mcl3_.position_changed.connect(set_pos);

		mcl3_.joystick_changed.connect([this](bool on){
			exception_catcher([&]{
				auto block = block_signals(joystick_);
				joystick_.setChecked(on);
				if(on) timer_.start(500);
			});
		});

		connect(&joystick_, &QPushButton::released, [this]{
			exception_catcher([&]{
				mcl3_.activate_joystick(joystick_.isChecked());
			});
		});

		connect(&reset_pos_, &QPushButton::released, [this]{
			exception_catcher([&]{
				mcl3_.set_position(0, 0, 0);
				std::cout << mcl3_.read_x() << ' ';
				std::cout << mcl3_.read_y() << ' ';
				std::cout << mcl3_.read_z() << std::endl;
			});
		});

		connect(&timer_, &QTimer::timeout, [this, set_pos]{
			if(!joystick_.isChecked()) return;

			exception_catcher([&]{
				auto pos = mcl3_.position();
				set_pos(pos[0], pos[1], pos[2]);
			});

			timer_.start(100);
		});
	}


}
