//-----------------------------------------------------------------------------
// Copyright (c) 2009-2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__bitmap__hpp_INCLUDED_
#define _linescan__bitmap__hpp_INCLUDED_

#include "rect.hpp"

#include <boost/container/vector.hpp>

#include <utility>
#include <stdexcept>
#include <sstream>


namespace linescan{


	template < typename T >
	using vector = boost::container::vector< T >;

	template < typename ValueType >
	class bitmap;


	template < typename ValueType >
	class offset_view{
	public:
		ValueType& operator()(std::size_t x, std::size_t y){
			return ref_(x_ + x, y_ + y);
		}

		ValueType const& operator()(std::size_t x, std::size_t y)const{
			return ref_(x_ + x, y_ + y);
		}

	private:
		offset_view(bitmap< ValueType >& ref, std::size_t x, std::size_t y):
			ref_(ref), x_(x), y_(y) {}

		bitmap< ValueType >& ref_;
		std::size_t const x_;
		std::size_t const y_;

	friend class bitmap< ValueType >;
	};


	template < typename ValueType >
	class const_offset_view{
	public:
		ValueType const& operator()(std::size_t x, std::size_t y)const{
			return ref_(x_ + x, y_ + y);
		}

	private:
		const_offset_view(bitmap< ValueType > const& ref, std::size_t x, std::size_t y):
			ref_(ref), x_(x), y_(y) {}

		bitmap< ValueType > const& ref_;
		std::size_t const x_;
		std::size_t const y_;

	friend class bitmap< ValueType >;
	};


	/// \brief A bitmap for data manipulation
	/// \tparam ValueType Type of the data that administrates the bitmap
	///
	template < typename ValueType >
	class bitmap{
	public:
		/// \brief Type of the data that administrates the bitmap
		using value_type = ValueType;

		/// \brief Type of points in the bitmap
		using point_type = point< std::size_t >;

		/// \brief Type of bitmap size
		using size_type = linescan::size< std::size_t >;

		/// \brief Type of a iterator for data
		using iterator = typename vector< value_type >::iterator;

		/// \brief Type of a iterator for const data
		using const_iterator = typename vector< value_type >::const_iterator;

		/// \brief Type of a reverse iterator for data
		using reverse_iterator = typename vector< value_type >::reverse_iterator;

		/// \brief Type of a reverse iterator for const data
		using const_reverse_iterator = typename vector< value_type >::const_reverse_iterator;

		/// \brief Type of a reference to data
		using reference = typename vector< value_type >::reference;

		/// \brief Type of a reference to const data
		using const_reference = typename vector< value_type >::const_reference;

		/// \brief Type of a pointer to data
		using pointer = typename vector< value_type >::pointer;

		/// \brief Type of a pointer to const data
		using const_pointer = typename vector< value_type >::const_pointer;



		/// \brief Constructs a blank bitmap
		bitmap() = default;

		/// \brief Constructs a bitmap by copying another one
		bitmap(bitmap const&) = default;

		/// \brief Constructs a bitmap by moving the data of another one
		bitmap(bitmap&&) = default;

		/// \brief Constructs a bitmap on position (0, 0), with size size.width and size.height, initialiese all values with value
		/// \throw std::out_of_range
		bitmap(size_type const& size, value_type const& value = value_type()):
			size_(size),
			data_(size_.point_count(), value)
		{
			throw_if_size_is_negative(size_);
		}

		/// \brief Constructs a bitmap on position (0, 0), with size size.width and size.height, initialiese all values with value
		/// \throw std::out_of_range
		template < typename InputIterator >
		bitmap(size_type const& size, InputIterator first, InputIterator last) :
			size_(size),
			data_(first, last)
		{
			throw_if_size_is_negative(size_);
			if (data_.size() != size_.point_count()){
				throw std::logic_error(
					"tools::bitmap constructor size (" +
					std::to_string(size_.width()) + "x" + std::to_string(size_.height()) +
					") and iterator range (" +
					std::to_string(data_.size()) +
					") are incompatible"
				);
			}
		}

		/// \brief Constructs a bitmap on position (0, 0), with size width and height, initialiese all values with value
		/// \throw std::out_of_range
		bitmap(std::size_t width, std::size_t height, value_type const& value = value_type()):
			size_(width, height),
			data_(size_.point_count(), value)
		{
			throw_if_size_is_negative(size_);
		}


		/// \brief Copy assignment
		bitmap& operator=(bitmap const& bitmap) = default;

		/// \brief Move assignment
		bitmap& operator=(bitmap&& bitmap) = default;


		/// \brief Get a iterator on begin of the data
		iterator begin(){
			return data_.begin();
		}

		/// \brief Get a const_iterator on begin of the data
		const_iterator begin()const{
			return data_.begin();
		}

		/// \brief Get a const_iterator on begin of the data
		const_iterator cbegin()const{
			return data_.cbegin();
		}

		/// \brief Get a iterator behind the data
		iterator end(){
			return data_.end();
		}

		/// \brief Get a const_iterator behind the data
		const_iterator end()const{
			return data_.end();
		}

		/// \brief Get a const_iterator behind the data
		const_iterator cend()const{
			return data_.cend();
		}


		/// \brief Get a reverse_iterator on end of the data
		reverse_iterator rbegin(){
			return data_.rbegin();
		}

		/// \brief Get a const_reverse_iterator on end of the data
		const_reverse_iterator rbegin()const{
			return data_.rbegin();
		}

		/// \brief Get a const_reverse_iterator on end of the data
		const_reverse_iterator crbegin()const{
			return data_.crbegin();
		}

		/// \brief Get a reverse_iterator before the data
		reverse_iterator rend(){
			return data_.rend();
		}

		/// \brief Get a const_reverse_iterator before the data
		const_reverse_iterator rend()const{
			return data_.rend();
		}

		/// \brief Get a const_reverse_iterator before the data
		const_reverse_iterator crend()const{
			return data_.crend();
		}


		/// \brief Resize the data field
		/// \attention All pointers and iterators to the data become invalid
		void resize(std::size_t width, std::size_t height, value_type const& value = value_type()){
			resize(size_type(width, height), value);
		}

		/// \brief Resize the data field
		/// \attention All pointers and iterators to the data become invalid
		void resize(size_type const& size, value_type const& value = value_type()){
			throw_if_size_is_negative(size);
			data_.resize(size.point_count(), value);
			size_ = size;
		}


		/// \brief Get the width
		std::size_t width()const{
			return size_.width();
		}

		/// \brief Get the height
		std::size_t height()const{
			return size_.height();
		}

		/// \brief Get the size
		size_type const size()const{
			return size_;
		}

		/// \brief Get the number of points in the bitmap
		std::size_t point_count()const{
			return size_.width() * size_.height();
		}


		/// \brief Get a pointer to data for direct manipulation
		value_type* data(){
			return const_cast< value_type* >(static_cast< const bitmap< value_type >& >(*this).data());
		}

		/// \brief Get a pointer to constant data for direct read
		value_type const* data()const{
			return data_.empty() ? 0 : &data_[0];
		}


		/// \brief Get a reference to the value by local coordinates
		/// \throw std::out_of_range in debug build
		reference operator()(std::size_t x, std::size_t y){
			return operator()(point_type(x, y));
		}

		/// \brief Get a reference to the value by local coordinates
		/// \throw std::out_of_range in debug build
		reference operator()(point_type const& point){
			throw_if_out_of_range(point);
			return do_point(point);
		}

		/// \brief Get the value by local coordinates
		/// \throw std::out_of_range in debug build
		const_reference operator()(std::size_t x, std::size_t y)const{
			return operator()(point_type(x, y));
		}

		/// \brief Get the value by local coordinates
		/// \throw std::out_of_range in debug build
		const_reference operator()(point_type const& point)const{
			throw_if_out_of_range(point);
			return do_point(point);
		}


		/// \brief Get a reference to the value by local coordinates
		auto offset_view(std::size_t x, std::size_t y){
			return linescan::offset_view< value_type >(*this, x, y);
		}

		/// \brief Get a reference to the value by local coordinates
		auto offset_view(point_type const& point){
			return offset_view(point.x(), point.y());
		}

		/// \brief Get the value by local coordinates
		auto offset_view(std::size_t x, std::size_t y)const{
			return linescan::const_offset_view< value_type >(*this, x, y);
		}

		/// \brief Get the value by local coordinates
		auto offset_view(point_type const& point)const{
			return offset_view(point.x(), point.y());
		}


		/// \brief Converts a lokal point in a index for direct data access
		/// \attention This function performs no range protection
		std::size_t data_pos(std::size_t x, std::size_t y)const{
			return data_pos(point_type(x, y));
		}

		/// \brief Converts a lokal point in a index for direct data access
		/// \attention This function performs no range protection
		std::size_t data_pos(point_type const& point)const{
			return point.y() * width() + point.x();
		}


	protected:
		/// \brief The rectangle for global position and size
		size_type size_;

		/// \brief The data field
		vector< value_type > data_;


		/// \brief Get a point without range protection
		reference do_point(point_type const& point){
			return data_[data_pos(point)];
		}

		/// \brief Get a point without range protection
		const_reference do_point(point_type const& point)const{
			return data_[data_pos(point)];
		}

		/// \brief Throws an exception, if the point is out of range
		void throw_if_out_of_range(point_type const&
			#ifdef DEBUG
			point
			#endif
		)const{
			#ifdef DEBUG
				if(!is_point_in_bitmap(*this, point)){
					std::ostringstream os;
					os << "tools::bitmap: std::out_of_range: point(x = " << point.x() << ", y = " << point.y() << ") is outside the bitmap (width = " << width() << ", height = " << height() << ")";
					throw std::out_of_range(os.str());
				}
			#endif
		}

		/// \brief Throws an exception if size is negative
		/// \throw std::out_of_range in debug build
		static void throw_if_size_is_negative(size_type const& size){
			if(!size.is_positive()){
				std::ostringstream os;
				os << "tools::bitmap: std::out_of_range: bitmap obtain negative size{" << size.width() << ", " << size.height() << "}";
				throw std::out_of_range(os.str());
			}
		}
	};


	template < typename ValueType >
	inline
	bool is_point_in_bitmap(bitmap< ValueType > const& image, typename bitmap< ValueType >::point_type const& point){
		if(
			point.x() <  0              ||
			point.x() >= image.width()  ||
			point.y() <  0              ||
			point.y() >= image.height()
		){
			return false;
		}
		return true;
	}


}

#endif
