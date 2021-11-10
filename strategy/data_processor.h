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

  	template <typename Number>
  	static MarketData getNewMarketData (types::String ticker, Number number) {
  		MarketData market_data;
  		market_data.ticker = std::move(ticker);
		market_data.quote.timestamp = timestamp::Timestamp();
		market_data.quote.value = types::Value(number);
		return market_data;
  	}
  };



}//!namespace

#endif //STRATEGY_TRADING_DATA_PROCESSOR_H
