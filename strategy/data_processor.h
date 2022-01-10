//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "curl_client/utils.h"
#include "types_declarations.h"
#include "quote.h"
#include "maps.h"
#include "coinbase.h"

#include <vector>

#ifndef STRATEGY_TRADING_DATA_PROCESSOR_H
#define STRATEGY_TRADING_DATA_PROCESSOR_H

namespace algo {


  struct MarketData {
	  //todo: make it OHLC available, as well as batches available
	  Ticker ticker;
	  Quote<types::Value, time_::Milliseconds> quote;
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
		  market_data.quote = types::Value(number);
		  market_data_batch.emplace_back(std::move(market_data));
	  }

	  template <typename Number>
	  void addNewMarketData ([[maybe_unused]] time_::Timestamp<time_::Milliseconds> ts, types::String ticker, Number number) {
		  MarketData market_data;
		  market_data.ticker = std::move(ticker);
		  market_data.quote = types::Value(number);
		  market_data_batch.emplace_back(std::move(market_data));
	  }

	  MarketDataBatch market_data_batch;
  };

  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
  using MarketData_ = std::pair<Ticker, Quote<QuoteType, Duration>>;

  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
  using MarketDataBatch_ = std::map<Ticker, Quote<QuoteType, Duration>>;

  namespace DataProcessor_ {

	template <typename Duration>
	time_::Timestamp<Duration> getTimestamp(types::String time_str) {
		return time_::fromString<Duration>(std::move(time_str), data::coinbase::input_fmt.c_str());
	}

	template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
	MarketData_<QuoteType, Duration> getNewMarketData (const types::String &t) {
		using namespace data::coinbase;
		auto tickers = utils::splitIntoWords(t, '-');
		assert (tickers.size() == 2);
		auto new_trade_data = processSingleTradeData(getSingleTradeData(tickers[0], tickers[1]));
		Quote<QuoteType, Duration> quote = types::Value (new_trade_data.price, 6); //todo:: get precision from a ticker data
		quote.timestamp = getTimestamp<Duration>(std::move(new_trade_data.time));

		return MarketData_<QuoteType, Duration>{t, std::move(quote)};
	}
  };


}//!namespace

#endif //STRATEGY_TRADING_DATA_PROCESSOR_H
