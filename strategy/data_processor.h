//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "quote.h"
#include "maps.h"

#include <vector>

#ifndef STRATEGY_TRADING_DATA_PROCESSOR_H
#define STRATEGY_TRADING_DATA_PROCESSOR_H

namespace algo {


  struct MarketData {
  	//todo: make it OHLC available, as well as batches available
  	Ticker ticker;
  	Quote quote;
  };

  //todo: make it pointer to timestamp, no copy - check Indicator file
  using MarketDataBatch = std::vector<MarketData>;

  struct DataProcessor {

	  MarketDataBatch getNewMarketData () {
		return std::move(market_data_batch);
  	}
	  template <typename Number>
	  void addNewMarketData (types::String ticker, Number number) {
		  MarketData market_data;
		  market_data.ticker = std::move(ticker);
		  market_data.quote.value = types::Value(number);
		  market_data_batch.emplace_back(std::move(market_data));
	  }

	  template <typename Number>
	  void addNewMarketData ([[maybe_unused]] timestamp::Timestamp<timestamp::Ms> ts, types::String ticker, Number number) {
		  MarketData market_data;
		  market_data.ticker = std::move(ticker);
		  market_data.quote.value = types::Value(number);
		  market_data_batch.emplace_back(std::move(market_data));
	  }

	  MarketDataBatch market_data_batch;
  };

}//!namespace

#endif //STRATEGY_TRADING_DATA_PROCESSOR_H
