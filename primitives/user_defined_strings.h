//
// Created by Andrey Solovyev on 09/11/2021.
//

#pragma once

#include <string>
#include <algorithm>
#include <cctype>

#ifndef STRATEGY_TRADING_STRING_H
#define STRATEGY_TRADING_STRING_H

namespace types {

#ifdef _WIN32
  using String = std::wstring;
#else
	using String = std::string;
#endif

  inline String trim(String &s) {
	  auto from = std::find_if(s.begin(), s.end(), [](int c) { return !std::isspace(c); });
	  auto to = std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base();

	  return (to > from) ? String(from, to) : "";
  }

}//!namespace


#endif //STRATEGY_TRADING_STRING_H
