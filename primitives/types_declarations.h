//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "value.h"
#include "maps.h"
#include "strings.h"


#ifndef STRATEGY_TRADING_TYPES_DECLARATIONS_H
#define STRATEGY_TRADING_TYPES_DECLARATIONS_H


namespace const_values {

	static const size_t DEFAULT_INDICATOR_SIZE = 30;

}//!namespace


namespace algo {

  using Ticker = types::String;

}//!namespace

#endif //STRATEGY_TRADING_TYPES_DECLARATIONS_H
