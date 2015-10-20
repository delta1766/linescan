//-----------------------------------------------------------------------------
// Copyright (c) 2009-2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__convolution__hpp_INCLUDED_
#define _linescan__convolution__hpp_INCLUDED_

#include "bitmap.hpp"
#include "matrix.hpp"


namespace linescan{


	template <
		typename ResultType,
		typename BitmapType,
		typename MatrixType,
		std::size_t Rows,
		std::size_t Columns
	>
	inline bitmap< ResultType > convolution(
		bitmap< BitmapType > const& image,
		matrix< MatrixType, Rows, Columns > const& matrix
	){
		auto const w = image.width() - matrix.width() + 1;
		auto const h = image.height() - matrix.height() + 1;
		bitmap< ResultType > result(w, h);

		for(std::size_t y = 0; y < h; ++y){
			for(std::size_t x = 0; x < w; ++x){
				for(std::size_t b = 0; b < matrix.height(); ++b){
					for(std::size_t a = 0; a < matrix.width(); ++a){
						result(x, y) += 
							static_cast< ResultType >(image(x + a, y + b)) *
							static_cast< ResultType >(matrix(a, b));
					}
				}
			}
		}

		return result;
	}

	template <
		typename ResultType,
		typename BitmapType,
		typename MatrixType1,
		typename MatrixType2,
		std::size_t Rows1,
		std::size_t Columns1,
		std::size_t Rows2,
		std::size_t Columns2
	>
	inline bitmap< ResultType > convolution(
		bitmap< BitmapType > const& image,
		matrix< MatrixType1, Rows1, Columns1 > const& matrix1,
		matrix< MatrixType2, Rows2, Columns2 > const& matrix2
	){
		return convolution< ResultType >(
			convolution< ResultType >(image, matrix1),
			matrix2
		);
	}


}


#endif
