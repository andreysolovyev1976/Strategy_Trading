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
#include <memory>

#ifndef STRATEGY_TRADING_DATA_PROCESSOR_H
#define STRATEGY_TRADING_DATA_PROCESSOR_H

namespace algo {

  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
  using MarketData = std::pair<Ticker, Quote<QuoteType, Duration>>;

  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
  using MarketDataBatch = std::map<Ticker, Quote<QuoteType, Duration>>;

  struct DataProcessor {
	  curl_client::Response response;
	  curl_client::Request request;

	  template <typename Duration>
	  time_::Timestamp<Duration> getTimestamp(types::String time_str) {
		  return time_::fromString<Duration>(std::move(time_str), data::coinbase::input_fmt.c_str());
	  }

	  //todo: don't like this, lots of tools will be declared and initialized each time, ie query_factory
	  // should be organized differently
	  // will work poorly in multithreading
	  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
	  MarketData<QuoteType, Duration> getNewMarketData (const TradingContract& trading_contract) {
		  return
				  std::visit(utils::overloaded {
						  []([[maybe_unused]] std::monostate arg) -> MarketData<QuoteType, Duration> {
							throw InvalidArgumentError(EXCEPTION_MSG("Unexpected Trading Contract - no Dex associated; "));
						  },
						  [this, &trading_contract](
								  [[maybe_unused]] trading_contract_base::dex_source::Coinbase arg) -> MarketData<QuoteType, Duration> {
							using namespace data::coinbase;
							Quote<QuoteType, Duration> quote;
							auto tickers = utils::splitIntoWords(trading_contract.ticker, '-');
							assert (tickers.size()==2);
							auto new_trade_data = processSingleTradeData(getSingleTradeData(tickers[0], tickers[1], response, request));
							quote.timestamp = getTimestamp<Duration>(std::move(new_trade_data.time));
							quote.value() = types::Value(new_trade_data.price, 6); //todo:: get precision from a ticker data
							return MarketData<QuoteType, Duration>{trading_contract.ticker, std::move(quote)};
						  },
						  [this, &trading_contract](
								  [[maybe_unused]] trading_contract_base::dex_source::Quipuswap arg) -> MarketData<QuoteType, Duration> {
							Quote<QuoteType, Duration> quote;
							std::visit(utils::overloaded {
									[]([[maybe_unused]] std::monostate arg) {
									  throw InvalidArgumentError(EXCEPTION_MSG("Unexpected Quipuswap - no Trade side selected; "));
									},
									[this, &quote, &trading_contract]([[maybe_unused]] trading_contract_base::quipuswap::SellXTZBuyToken arg) {
									  tezos::quipuswap::TokenPair<Duration> token;
									  token.pair_address = trading_contract.ticker;
									  token = tezos::quipuswap::estimateTezToToken(std::move(token), response, request);
									  quote.timestamp = std::move(token.timestamp);
									  quote.value() = std::move(token.current_price_tez);
									},
									[this, &quote, &trading_contract]([[maybe_unused]] trading_contract_base::quipuswap::BuyXTZSellToken arg) {
									  tezos::quipuswap::TokenPair<Duration> token;
									  token.pair_address = trading_contract.ticker;
									  token = tezos::quipuswap::estimateTokenToTez(std::move(token), response, request);
									  quote.timestamp = std::move(token.timestamp);
									  quote.value() = std::move(token.current_price_token);
									}
							}, trading_contract.quipuswap_trade_side);
							return MarketData<QuoteType, Duration>{trading_contract.ticker, std::move(quote)};
						  }
				  }, trading_contract.dex);
	  }//!func
  };

  using DataProcessorPtr = std::shared_ptr<DataProcessor>;

}//!namespace

#endif //STRATEGY_TRADING_DATA_PROCESSOR_H
