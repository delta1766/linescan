//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/linescan
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _tools_mask_non_print_hpp_INCLUDED_
#define _tools_mask_non_print_hpp_INCLUDED_

#include <string>


namespace linescan{


	inline std::string mask_non_print(std::string const& str){
		std::string result;
		for(auto c: str){
			switch(c){
				case 0: result += "\\0"; break;
				case 1: result += "\\x01"; break;
				case 2: result += "\\x02"; break;
				case 3: result += "\\x03"; break;
				case 4: result += "\\x04"; break;
				case 5: result += "\\x05"; break;
				case 6: result += "\\x06"; break;
				case 7: result += "\\a"; break;
				case 8: result += "\\b"; break;
				case 9: result += "\\t"; break;
				case 10: result += "\\n"; break;
				case 11: result += "\\v"; break;
				case 12: result += "\\f"; break;
				case 13: result += "\\r"; break;
				case 14: result += "\\x0E"; break;
				case 15: result += "\\x0F"; break;
				case 16: result += "\\x10"; break;
				case 17: result += "\\x11"; break;
				case 18: result += "\\x12"; break;
				case 19: result += "\\x13"; break;
				case 20: result += "\\x14"; break;
				case 21: result += "\\x15"; break;
				case 22: result += "\\x16"; break;
				case 23: result += "\\x17"; break;
				case 24: result += "\\x18"; break;
				case 25: result += "\\x19"; break;
				case 26: result += "\\x1A"; break;
				case 27: result += "\\e"; break;
				case 28: result += "\\x1C"; break;
				case 29: result += "\\x1D"; break;
				case 30: result += "\\x1E"; break;
				case 31: result += "\\x1F"; break;
				case '\\': result += "\\\\"; break;
				default: result += c;
			}
		}
		return result;
	}


}


#endif
