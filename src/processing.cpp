//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <linescan/processing.hpp>

#include <mitrax/convolution.hpp>
#include <mitrax/pass_in.hpp>

#include <cmath>


namespace linescan{


	mitrax::std_bitmap< bool > erode(
		mitrax::std_bitmap< bool > const& image,
		std::size_t size,
		bool border_value
	){
		if(size == 0) return image;
		return mitrax::pass_in(image.dims(),
			mitrax::transform_per_view([](auto const& m){
				bool result = false;

				for(std::size_t y = 0; y < m.rows(); ++y){
					for(std::size_t x = 0; x < m.cols(); ++x){
						if(!m(x, y)) continue;
						result = true;
					}
				}

				return result;
			}, mitrax::cols(size + 1), mitrax::rows(size + 1), image),
			border_value
		);
	}

	mitrax::std_bitmap< std::uint8_t > gauss(
		mitrax::std_bitmap< std::uint8_t > const& image,
		std::size_t size,
		float variance
	){
		// create kernel
		auto vc = mitrax::make_vector_v< float >(mitrax::rows(size));
		auto vr = mitrax::make_vector_v< float >(mitrax::cols(size));

		float sum = 0;
		for(std::size_t i = 0; i < size; ++i){
			auto x = i - size / 2.f + .5f;

			auto value =
				std::exp(- ((x * x) / (2 * variance * variance))) /
				(variance * std::sqrt(2 * 3.14159f));

			vc[i] = value;

			sum += value;
		}

		for(std::size_t i = 0; i < size; ++i){
			vc[i] /= sum;
			vr[i] = vc[i];
		}

		// apply kernel
		return transform([](auto v){
			return static_cast< std::uint8_t >(v);
		}, convolution(image, vc, vr));
	}


}
