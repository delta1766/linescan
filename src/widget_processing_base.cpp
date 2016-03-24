//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/widget_processing_base.hpp>
#include <linescan/processing.hpp>
#include <linescan/to_image.hpp>


namespace linescan{


	widget_processing_base::widget_processing_base(camera& cam):
		original_(tr("Original image")),
		binarized_(tr("Binarized image")),
		eroded_(tr("Eroded image")),
		binarize_threashold_l_(tr("Threashold")),
		erode_l_(tr("Erode")),
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

		binarize_threashold_.setValue(255);
		erode_.setValue(2);

		binarize_threashold_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		erode_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		sub_pixel_l_.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		hlayout_.addLayout(&glayout_);
		hlayout_.addWidget(&image_, 1);

		radio_buttons_.addButton(&original_);
		radio_buttons_.addButton(&binarized_);
		radio_buttons_.addButton(&eroded_);

		original_.setChecked(true);


		connect(&original_, &QRadioButton::released, [this]{
			if(!original_.isChecked()) return;

			image_.reset_processor();
		});

		connect(&binarized_, &QRadioButton::released, [this]{
			if(!binarized_.isChecked()) return;

			image_.set_processor(
				[this](mitrax::raw_bitmap< std::uint8_t >&& bitmap){
					return std::pair< QImage, QImage >{
						to_image(binarized(bitmap)),
						QImage()
					};
				});
		});

		connect(&eroded_, &QRadioButton::released, [this]{
			if(!eroded_.isChecked()) return;

			image_.set_processor(
				[this](mitrax::raw_bitmap< std::uint8_t >&& bitmap){
					return std::pair< QImage, QImage >{
						to_image(eroded(bitmap)),
						QImage()
					};
				});
		});

		setLayout(&hlayout_);
	}

	std::uint8_t widget_processing_base::get_threashold()const{
		return static_cast< std::uint8_t >(binarize_threashold_.value());
	}

	std::uint8_t widget_processing_base::get_erode()const{
		return static_cast< std::uint8_t >(erode_.value());
	}

	mitrax::raw_bitmap< bool > widget_processing_base::binarized(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap
	)const{
		return binarize(bitmap, get_threashold());
	}

	mitrax::raw_bitmap< bool > widget_processing_base::eroded(
		mitrax::raw_bitmap< std::uint8_t > const& bitmap
	)const{
		return erode(binarized(bitmap), get_erode());
	}


}
