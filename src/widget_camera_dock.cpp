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


	std::size_t scale_to_slide(double value, double min, double inc){
		return static_cast< std::size_t >((value - min) / inc + 0.5);
	}

	template < typename O >
	std::size_t scale_to_slide(double value, O const& object){
		auto min = object.minimum();
		auto inc = object.singleStep();

		return scale_to_slide(value, min, inc);
	}

	template < typename T >
	T scale_from_slide(double value, double min, double inc){
		return static_cast< T >(value * inc + min);
	}

	template < typename T, typename O >
	T scale_from_slide(double value, O const& object){
		auto min = object.minimum();
		auto inc = object.singleStep();

		return scale_from_slide< T >(value, min, inc);
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

				set_framerate_ranges();
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

				set_exposure_ranges();
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

	void widget_camera_dock::set_pixelclock_ranges(){
		auto min_max_inc = cam_.pixelclock_min_max_inc();
		auto min = min_max_inc[0];
		auto max = min_max_inc[1];
		auto inc = min_max_inc[2];
		auto value = cam_.pixelclock();

		auto int_max = scale_to_slide(max, min, inc);
		auto int_value = scale_to_slide(value, min, inc);

		pixelclock_v_.setRange(min, max);
		pixelclock_v_.setSingleStep(inc);
		pixelclock_v_.setValue(value);

		pixelclock_.setRange(0, int_max);
		pixelclock_.setSingleStep(1);
		pixelclock_.setValue(int_value);

		pixelclock_ml_.setText(QString(tr("%1 MHz")).arg(min));
		pixelclock_xl_.setText(QString(tr("%1 MHz")).arg(max));
		pixelclock_il_.setText(QString(tr("(%1 MHz)")).arg(inc));
	}

	void widget_camera_dock::set_framerate_ranges(){
		auto min_max_inc = cam_.framerate_min_max_inc();
		auto min = min_max_inc[0];
		auto max = min_max_inc[1];
		auto inc = min_max_inc[2];
		auto value = cam_.framerate();

		auto int_max = scale_to_slide(max, min, inc);
		auto int_value = scale_to_slide(value, min, inc);

		framerate_v_.setRange(min, max);
		framerate_v_.setSingleStep(inc);
		framerate_v_.setValue(value);

		framerate_.setRange(0, int_max);
		framerate_.setSingleStep(1);
		framerate_.setValue(int_value);

		framerate_ml_.setText(QString(tr("%1 fps")).arg(min, 0, 'f', 2));
		framerate_xl_.setText(QString(tr("%1 fps")).arg(max, 0, 'f', 2));
		framerate_il_.setText(QString(tr("(%1 fps)")).arg(inc, 0, 'g', 2));
	}

	void widget_camera_dock::set_exposure_ranges(){
		auto min_max_inc = cam_.exposure_in_ms_min_max_inc();
		auto min = min_max_inc[0];
		auto max = min_max_inc[1];
		auto inc = min_max_inc[2];
		auto value = cam_.exposure_in_ms();

		auto int_max = scale_to_slide(max, min, inc);
		auto int_value = scale_to_slide(value, min, inc);

		exposure_v_.setRange(min, max);
		exposure_v_.setSingleStep(inc);
		exposure_v_.setValue(value);

		exposure_.setRange(0, int_max);
		exposure_.setSingleStep(1);
		exposure_.setValue(int_value);

		exposure_ml_.setText(QString(tr("%1 ms")).arg(min, 0, 'f', 2));
		exposure_xl_.setText(QString(tr("%1 ms")).arg(max, 0, 'f', 2));
		exposure_il_.setText(QString(tr("(%1 ms)")).arg(inc, 0, 'g', 2));
	}

	void widget_camera_dock::set_gain_ranges(){
		auto min = 0;
		auto max = 100;
		auto inc = 1;
		auto value = cam_.gain_in_percent();

		gain_v_.setRange(min, max);
		gain_v_.setSingleStep(inc);
		gain_v_.setValue(value);

		gain_.setRange(min, max);
		gain_.setSingleStep(inc);
		gain_.setValue(value);

		gain_ml_.setText(QString(tr("%1%")).arg(min));
		gain_xl_.setText(QString(tr("%1%")).arg(max));
		gain_il_.setText(QString(tr("(%1%)")).arg(inc));
	}

	void widget_camera_dock::set_ranges(){
		set_pixelclock_ranges();
		set_framerate_ranges();
		set_exposure_ranges();
		set_gain_ranges();

		gain_boost_.setChecked(cam_.gain_boost());
	}


}
