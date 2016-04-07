//-----------------------------------------------------------------------------
// Copyright (c) 2016 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _linescan__block_signals__hpp_INCLUDED_
#define _linescan__block_signals__hpp_INCLUDED_


namespace linescan{


	template < typename T >
	class block_signals_t{
	public:
		block_signals_t(T& item):
			item_(&item){
				item_->blockSignals(true);
			}

		block_signals_t(block_signals_t&& v)noexcept:
			item_(v.item_){
			v.item_ = nullptr;
		}

		block_signals_t(block_signals_t const&) = delete;


		~block_signals_t(){
			if(item_) item_->blockSignals(false);
		}


	private:
		T* item_;
	};

	template < typename T >
	auto block_signals(T& item){
		return block_signals_t< T >(item);
	}


}


#endif
