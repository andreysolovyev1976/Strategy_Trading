//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "curl_client/utils.h"
#include "types_declarations.h"
#include "maps.h"

#include "quipuswap.h"
#include "coinbase.h"

#include "quote.h"
#include "trading_contract.h"

#include <vector>
#include <mutex>

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

	//todo: don't like this, lots of tools will be declared and initialized each time, ie query_factory
	// should be organized differently
	// will work poorly in multithreading
	template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
	MarketData_<QuoteType, Duration> getNewMarketData (const TradingContract& trading_contract) {
		return
				std::visit(utils::overloaded {
						[]([[maybe_unused]] std::monostate arg) -> MarketData_<QuoteType, Duration> {
						  throw InvalidArgumentError(EXCEPTION_MSG("Unexpected Trading Contract - no Dex associated; "));
						},
						[&trading_contract](
								[[maybe_unused]] trading_contract_base::dex_source::Coinbase arg) -> MarketData_<QuoteType, Duration> {
						  using namespace data::coinbase;
						  auto tickers = utils::splitIntoWords(trading_contract.ticker, '-');
						  assert (tickers.size()==2);
						  auto new_trade_data = processSingleTradeData(getSingleTradeData(tickers[0], tickers[1]));
						  Quote<QuoteType, Duration> quote = types::Value(new_trade_data.price,
								  6); //todo:: get precision from a ticker data
						  quote.timestamp = getTimestamp<Duration>(std::move(new_trade_data.time));
						  return MarketData_<QuoteType, Duration>{trading_contract.ticker, std::move(quote)};
						},
						[&trading_contract](
								[[maybe_unused]] trading_contract_base::dex_source::Quipuswap arg) -> MarketData_<QuoteType, Duration> {
						  Quote<QuoteType, Duration> quote;
						  std::visit(utils::overloaded {
								  []([[maybe_unused]] std::monostate arg) {
									throw InvalidArgumentError(EXCEPTION_MSG("Unexpected Quipuswap - no Trade side selected; "));
								  },
								  [&quote, &trading_contract]([[maybe_unused]] trading_contract_base::quipuswap::SellXTZBuyToken arg) {
									tezos::quipuswap::TokenPair<Duration> token;
									token.pair_address = trading_contract.ticker;
									token = tezos::quipuswap::estimateTezToToken(std::move(token));
									quote.timestamp = std::move(token.timestamp);
									quote.value_ = std::move(token.current_price_tez);

								  },
								  [&quote, &trading_contract]([[maybe_unused]] trading_contract_base::quipuswap::BuyXTZSellToken arg) {
									tezos::quipuswap::TokenPair<Duration> token;
									token.pair_address = trading_contract.ticker;
									token = tezos::quipuswap::estimateTokenToTez(std::move(token));
									quote.timestamp = std::move(token.timestamp);
									quote.value_ = std::move(token.current_price_token);
								  }
						  }, trading_contract.quipuswap_trade_side);
						  return MarketData_<QuoteType, Duration>{trading_contract.ticker, std::move(quote)};
						}
				}, trading_contract.dex);
	}//!func
  }//!namespace
}//!namespace

#endif //STRATEGY_TRADING_DATA_PROCESSOR_H
