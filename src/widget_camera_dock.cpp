//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_camera_dock.hpp>
#include <linescan/exception_catcher.hpp>


namespace linescan{


	std::size_t scale_to_slide(double value, double min, double step){
		return static_cast< std::size_t >((value - min) / step + 0.5);
	}

	template < typename O >
	std::size_t scale_to_slide(double value, O const& object){
		auto min = object.minimum();
		auto step = object.singleStep();

		return scale_to_slide(value, min, step);
	}

	template < typename T >
	T scale_from_slide(double value, double min, double step){
		return static_cast< T >(value * step + min);
	}

	template < typename T, typename O >
	T scale_from_slide(double value, O const& object){
		auto min = object.minimum();
		auto step = object.singleStep();

		return scale_from_slide< T >(value, min, step);
	}

	widget_camera_dock::widget_camera_dock(camera& cam):
		QDockWidget(tr("Camera settings")),
		cam_(cam),
		pixelclock_l_(tr("Pixelclock")),
		framerate_l_(tr("Framerate")),
		exposure_l_(tr("Exposure")),
		gain_l_(tr("Gain")),
		gain_boost_l_(tr("Gain boost"))
	{
		pixelclock_v_.setSuffix(tr(" MHz"));
		framerate_v_.setSuffix(tr(" fps"));
		exposure_v_.setSuffix(tr(" ms"));
		gain_v_.setSuffix("%");

		pixelclock_.setOrientation(Qt::Horizontal);
		framerate_.setOrientation(Qt::Horizontal);
		exposure_.setOrientation(Qt::Horizontal);
		gain_.setOrientation(Qt::Horizontal);

		pixelclock_ml_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		pixelclock_xl_.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		pixelclock_il_.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

		framerate_ml_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		framerate_xl_.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		framerate_il_.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

		exposure_ml_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		exposure_xl_.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		exposure_il_.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

		gain_ml_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		gain_xl_.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		gain_il_.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

		layout_.addWidget(&pixelclock_l_, 0, 0);
		layout_.addWidget(&pixelclock_v_, 0, 1);
		layout_.addWidget(&pixelclock_ml_, 0, 2);
		layout_.addWidget(&pixelclock_, 0, 3);
		layout_.addWidget(&pixelclock_xl_, 0, 4);
		layout_.addWidget(&pixelclock_il_, 0, 5);

		layout_.addWidget(&framerate_l_, 1, 0);
		layout_.addWidget(&framerate_v_, 1, 1);
		layout_.addWidget(&framerate_ml_, 1, 2);
		layout_.addWidget(&framerate_, 1, 3);
		layout_.addWidget(&framerate_xl_, 1, 4);
		layout_.addWidget(&framerate_il_, 1, 5);

		layout_.addWidget(&exposure_l_, 2, 0);
		layout_.addWidget(&exposure_v_, 2, 1);
		layout_.addWidget(&exposure_ml_, 2, 2);
		layout_.addWidget(&exposure_, 2, 3);
		layout_.addWidget(&exposure_xl_, 2, 4);
		layout_.addWidget(&exposure_il_, 2, 5);

		layout_.addWidget(&gain_l_, 3, 0);
		layout_.addWidget(&gain_v_, 3, 1);
		layout_.addWidget(&gain_ml_, 3, 2);
		layout_.addWidget(&gain_, 3, 3);
		layout_.addWidget(&gain_xl_, 3, 4);
		layout_.addWidget(&gain_il_, 3, 5);

		layout_.addWidget(&gain_boost_l_, 4, 0);
		layout_.addWidget(&gain_boost_, 4, 1);

		layout_.setColumnStretch(3, 1);

		set_ranges();


		constexpr auto int_valueChanged =
			static_cast< void(QSpinBox::*)(int) >(&QSpinBox::valueChanged);
	
		constexpr auto double_valueChanged =
			static_cast< void(QDoubleSpinBox::*)(double) >(
				&QDoubleSpinBox::valueChanged
			);

		connect(&pixelclock_v_, int_valueChanged, [this](int value){
			exception_catcher([&]{
				pixelclock_.setValue(scale_to_slide(value, pixelclock_v_));

				cam_.set_pixelclock(value);
				set_ranges();
			});
		});

		connect(&pixelclock_, &QSlider::valueChanged, [this](int value){
			exception_catcher([&]{
				pixelclock_v_.setValue(
					scale_from_slide< std::uint32_t >(value, pixelclock_v_)
				);
			});
		});

		connect(&framerate_v_, double_valueChanged, [this](double value){
			exception_catcher([&]{
				framerate_.setValue(scale_to_slide(value, framerate_v_));

				cam_.set_framerate(value);
				set_ranges();
			});
		});

		connect(&framerate_, &QSlider::valueChanged, [this](int value){
			exception_catcher([&]{
				framerate_v_.setValue(
					scale_from_slide< double >(value, framerate_v_)
				);
			});
		});

		connect(&exposure_v_, double_valueChanged, [this](double value){
			exception_catcher([&]{
				exposure_.setValue(scale_to_slide(value, exposure_v_));

				cam_.set_exposure(value);
			});
		});

		connect(&exposure_, &QSlider::valueChanged, [this](int value){
			exception_catcher([&]{
				exposure_v_.setValue(
					scale_from_slide< double >(value, exposure_v_)
				);
			});
		});

		connect(&gain_v_, int_valueChanged, [this](int value){
			exception_catcher([&]{
				gain_.setValue(value);

				cam_.set_gain(value);
			});
		});

		connect(&gain_, &QSlider::valueChanged, [this](int value){
			exception_catcher([&]{
				gain_v_.setValue(value);
			});
		});

		connect(&gain_boost_, &QCheckBox::toggled, [this](bool checked){
			exception_catcher([&]{
				cam_.set_gain_boost(checked);
			});
		});

		widget_.setLayout(&layout_);
		setWidget(&widget_);
	}

	void widget_camera_dock::set_ranges(){
		auto pixelclock_min = cam_.pixelclock_min();
		auto pixelclock_max = cam_.pixelclock_max();
		auto pixelclock_step = cam_.pixelclock_inc();
		auto pixelclock_value = cam_.pixelclock();

		auto framerate_min = cam_.framerate_min();
		auto framerate_max = cam_.framerate_max();
		auto framerate_step = cam_.framerate_inc();
		auto framerate_value = cam_.framerate();

		auto exposure_min = cam_.exposure_in_ms_min();
		auto exposure_max = cam_.exposure_in_ms_max();
		auto exposure_step = cam_.exposure_in_ms_inc();
		auto exposure_value = cam_.exposure_in_ms();

		auto gain_min = 0;
		auto gain_max = 100;
		auto gain_step = 1;
		auto gain_value = cam_.gain_in_percent();


		auto pixelclock_int_max =
			scale_to_slide(pixelclock_max, pixelclock_min, pixelclock_step);

		auto pixelclock_int_value =
			scale_to_slide(pixelclock_value, pixelclock_min, pixelclock_step);

		pixelclock_v_.setRange(pixelclock_min, pixelclock_max);
		pixelclock_v_.setSingleStep(pixelclock_step);
		pixelclock_v_.setValue(pixelclock_value);

		pixelclock_.setRange(0, pixelclock_int_max);
		pixelclock_.setSingleStep(1);
		pixelclock_.setValue(pixelclock_int_value);

		pixelclock_ml_.setText(QString(tr("%1 MHz")).arg(pixelclock_min));
		pixelclock_xl_.setText(QString(tr("%1 MHz")).arg(pixelclock_max));
		pixelclock_il_.setText(QString(tr("(%1 MHz)")).arg(pixelclock_step));


		auto framerate_int_max =
			scale_to_slide(framerate_max, framerate_min, framerate_step);

		auto framerate_int_value =
			scale_to_slide(framerate_value, framerate_min, framerate_step);

		framerate_v_.setRange(framerate_min, framerate_max);
		framerate_v_.setSingleStep(framerate_step);
		framerate_v_.setValue(framerate_value);

		framerate_.setRange(0, framerate_int_max);
		framerate_.setSingleStep(1);
		framerate_.setValue(framerate_int_value);

		framerate_ml_.setText(
			QString(tr("%1 fps")).arg(framerate_min, 0, 'f', 2));
		framerate_xl_.setText(
			QString(tr("%1 fps")).arg(framerate_max, 0, 'f', 2));
		framerate_il_.setText(
			QString(tr("(%1 fps)")).arg(framerate_step, 0, 'g', 2));


		auto exposure_int_max =
			scale_to_slide(exposure_max, exposure_min, exposure_step);

		auto exposure_int_value =
			scale_to_slide(exposure_value, exposure_min, exposure_step);

		exposure_v_.setRange(exposure_min, exposure_max);
		exposure_v_.setSingleStep(exposure_step);
		exposure_v_.setValue(exposure_value);

		exposure_.setRange(0, exposure_int_max);
		exposure_.setSingleStep(1);
		exposure_.setValue(exposure_int_value);

		exposure_ml_.setText(
			QString(tr("%1 Hz")).arg(exposure_min, 0, 'f', 2));
		exposure_xl_.setText(
			QString(tr("%1 Hz")).arg(exposure_max, 0, 'f', 2));
		exposure_il_.setText(
			QString(tr("(%1 Hz)")).arg(exposure_step, 0, 'g', 2));


		gain_v_.setRange(gain_min, gain_max);
		gain_v_.setSingleStep(gain_step);
		gain_v_.setValue(gain_value);

		gain_.setRange(gain_min, gain_max);
		gain_.setSingleStep(gain_step);
		gain_.setValue(gain_value);

		gain_ml_.setText(QString(tr("%1%")).arg(gain_min));
		gain_xl_.setText(QString(tr("%1%")).arg(gain_max));
		gain_il_.setText(QString(tr("(%1%)")).arg(gain_step));


		gain_boost_.setChecked(cam_.gain_boost());
	}


}
