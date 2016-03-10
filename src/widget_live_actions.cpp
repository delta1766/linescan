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
#include <linescan/binarize.hpp>
#include <linescan/linear_function.hpp>
#include <linescan/calc_top_distance_line.hpp>
#include <linescan/erode.hpp>
#include <linescan/draw.hpp>
#include <linescan/point.hpp>


namespace linescan{


	widget_live_actions::widget_live_actions(
		camera& cam,
		message_callback_type const& message
	):
		save_count_(0),
		message_(message),
		original_(tr("Original image")),
		binarized_(tr("Binarized image")),
		eroded_(tr("Eroded image")),
		line_(tr("Line image")),
		binarize_threashold_l_(tr("Threashold")),
		erode_l_(tr("Erode")),
		sub_pixel_l_(tr("Subpixel")),
		save_(tr("Save")),
		image_(cam)
	{
		binarize_threashold_.setRange(0, 255);
		erode_.setRange(0, 10);

		glayout_.addWidget(&original_, 0, 0, 1, 2);
		glayout_.addWidget(&binarized_, 1, 0, 1, 2);
		glayout_.addWidget(&binarize_threashold_l_, 2, 0, 1, 1);
		glayout_.addWidget(&binarize_threashold_, 2, 1, 1, 1);
		glayout_.addWidget(&eroded_, 3, 0, 1, 2);
		glayout_.addWidget(&erode_l_, 4, 0, 1, 1);
		glayout_.addWidget(&erode_, 4, 1, 1, 1);
		glayout_.addWidget(&line_, 5, 0, 1, 2);
		glayout_.addWidget(&sub_pixel_l_, 6, 0, 1, 1);
		glayout_.addWidget(&sub_pixel_, 6, 1, 1, 1);
		glayout_.addWidget(&save_, 7, 0, 1, 2);
		glayout_.setRowStretch(8, 1);

		binarize_threashold_.setValue(255);
		erode_.setValue(3);

		binarize_threashold_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		erode_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		sub_pixel_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		hlayout_.addLayout(&glayout_);
		hlayout_.addWidget(&image_, 1);

		original_.setChecked(true);


		connect(&original_, &QRadioButton::released,
			[this]{
				if(!original_.isChecked()) return;

				image_.reset_processor();
			});

		connect(&binarized_, &QRadioButton::released,
			[this]{
				if(!binarized_.isChecked()) return;

				image_.set_processor(
					[this](
						mitrax::raw_bitmap< std::uint8_t >&& bitmap
					){
						return std::pair< QImage, QImage >{
							to_image(binarized(bitmap)),
							QImage()
						};
					});
			});

		connect(&eroded_, &QRadioButton::released,
			[this]{
				if(!eroded_.isChecked()) return;

				image_.set_processor(
					[this](
						mitrax::raw_bitmap< std::uint8_t >&& bitmap
					){
						return std::pair< QImage, QImage >{
							to_image(eroded(bitmap)),
							QImage()
						};
					});
			});

		connect(&line_, &QRadioButton::released,
			[this]{
				if(!line_.isChecked()) return;

				image_.set_processor(
					[this](
						mitrax::raw_bitmap< std::uint8_t >&& bitmap
					){
						auto line = calc_top_distance_line(eroded(bitmap));

						return std::pair< QImage, QImage >{
							[this, &line, &bitmap]{
								if(is_sub_pixel()){
									return to_image(draw_top_distance_line(
										line, bitmap.cols(), bitmap.rows()
									));
								}
									
								return to_image(draw_top_distance_line_student(
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

			message_(QString(tr("Save image '%1'.")).arg(name));
			image.save(name, "PNG");
		});

		setLayout(&hlayout_);
	}

	std::uint8_t widget_live_actions::get_threashold()const{
		return static_cast< std::uint8_t >(binarize_threashold_.value());
	}

	std::uint8_t widget_live_actions::get_erode()const{
		return static_cast< std::uint8_t >(erode_.value());
	}

	bool widget_live_actions::is_sub_pixel()const{
		return sub_pixel_.isChecked();
	}

	mitrax::raw_bitmap< bool > widget_live_actions::binarized(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap
	)const{
		return binarize(bitmap, get_threashold());
	}

	mitrax::raw_bitmap< bool > widget_live_actions::eroded(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap
	)const{
		return erode(binarized(bitmap), get_erode());
	}


}
