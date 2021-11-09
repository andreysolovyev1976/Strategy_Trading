//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "quote.h"

#ifndef STRATEGY_TRADING_DATA_PROCESSOR_H
#define STRATEGY_TRADING_DATA_PROCESSOR_H

namespace algo {

  struct MarketData {
  	//todo: make it OHLC available, as well as batches available
  	Ticker ticker;
  	Quote quote;
  };

  struct DataProcessor {
  	template <typename Input>
  	static MarketData getNewMarketData (Input &&input);
  };



}//!namespace

#endif //STRATEGY_TRADING_DATA_PROCESSOR_H
