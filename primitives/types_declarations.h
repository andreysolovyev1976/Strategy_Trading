//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "value.h"
#include "maps.h"

#include <string>


#ifndef STRATEGY_TRADING_TYPES_DECLARATIONS_H
#define STRATEGY_TRADING_TYPES_DECLARATIONS_H

namespace types {

#ifdef _WIN32
  using String = std::wstring;
#else
  using String = std::string;
#endif


}//!namespace


namespace const_values {

	static const size_t DEFAULT_INDICATOR_SIZE = 30;

}//!namespace


namespace algo {

  using Ticker = types::String;

}//!namespace

#endif //STRATEGY_TRADING_TYPES_DECLARATIONS_H
