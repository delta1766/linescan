//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_live_actions.hpp>
#include <linescan/to_image.hpp>
#include <linescan/calc_laser_line.hpp>
#include <linescan/draw.hpp>


namespace linescan{


	widget_live_actions::widget_live_actions(
		camera& cam,
		message_callback_type const& message
	):
		widget_processing_base(cam),
		message_(message),
		save_count_(0),
		line_(tr("Line image")),
		sub_pixel_l_(tr("Subpixel")),
		save_(tr("Save"))
	{
		radio_buttons_.addButton(&line_);

		glayout_.addWidget(&line_, 5, 0, 1, 2);
		glayout_.addWidget(&sub_pixel_l_, 6, 0, 1, 1);
		glayout_.addWidget(&sub_pixel_, 6, 1, 1, 1);
		glayout_.addWidget(&save_, 7, 0, 1, 2);
		glayout_.setRowStretch(8, 1);

		sub_pixel_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);


		connect(&line_, &QRadioButton::released, [this]{
			if(!line_.isChecked()) return;

			image_.set_processor(
				[this](mitrax::raw_bitmap< std::uint8_t >&& bitmap){
					auto line = calc_laser_line(
						bitmap, get_threashold(), get_erode()
					);

					return std::pair< QImage, QImage >{
						[this, &line, &bitmap]{
							if(is_sub_pixel()){
								return to_image(draw_laser_line(
									line, bitmap.cols(), bitmap.rows()
								));
							}

							return to_image(draw_laser_line_student(
								line, bitmap.cols(), bitmap.rows()
							));
						}(),
						QImage()
					};
				});
		});

		connect(&save_, &QPushButton::released, [this]{
			auto image = image_.image();

			auto name = QString("live_%1.png")
				.arg(save_count_, 4, 10, QLatin1Char('0'));

			++save_count_;

			message_(tr("Save image '%1'.").arg(name));
			image.save(name, "PNG");
		});
	}

	bool widget_live_actions::is_sub_pixel()const{
		return sub_pixel_.isChecked();
	}


}
