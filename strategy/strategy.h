//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "rule.h"
#include "trade.h"


#ifndef STRATEGY_TRADING_STRATEGY_H
#define STRATEGY_TRADING_STRATEGY_H

namespace algo {

namespace strategy_base {

  template <typename MarketData>
  using IsMarketData = std::enable_if_t<
  		std::disjunction_v<
  		std::is_same<types::Value, MarketData>,
		std::is_same<algo::Quote, MarketData>
//		, std::is_same<std::vector<types::Value>, IndicatorType>,
//  		std::is_same<std::vector<algo::Quote>, IndicatorType>
  		>,
  		bool>;
}//!namespace


  class Strategy {
  public:
	  Strategy () = default;

	  void addIndicator(); //reference data
	  void addSignal(); //special relations of market data and reference data
	  void addRule (); //actions, based on relations

	  types::String getIndicators();
	  types::String getSignals();
	  types::String getRules();

	  template <typename MarketData, strategy_base::IsMarketData<MarketData> = true>
	  Trade ruleSignal (MarketData market_data);

  private:
	  types::String name_;
	  uint32_t id_;
	  std::vector<trade_base::Ticker> tickers_;
	  std::vector<Rule> rules_;
	  bool is_initialized_ = false;

	  void applyRule();
  };

}//!namespace
#endif //STRATEGY_TRADING_STRATEGY_H
