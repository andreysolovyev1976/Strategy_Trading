//
// Created by Andrey Solovyev on 09/11/2021.
//

#pragma once

#include <string>


#ifndef STRATEGY_TRADING_STRING_H
#define STRATEGY_TRADING_STRING_H

namespace types {

#ifdef _WIN32
  using String = std::wstring;
#else
	using String = std::string;
#endif


}//!namespace


#endif //STRATEGY_TRADING_STRING_H
