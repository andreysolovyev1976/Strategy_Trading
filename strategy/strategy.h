//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "data_processor.h"

#include "rule.h"
#include "trade.h"
#include "account.h"

#include <map>

#ifndef STRATEGY_TRADING_STRATEGY_H
#define STRATEGY_TRADING_STRATEGY_H

namespace algo {

  namespace strategy_base {

  }//!namespace


  class Strategy {
  public:
	  Strategy () = default;
	  Strategy (Account& account);

	  void addIndicator(const types::String &label); //reference data
	  void addSignal(); //special relations of market data and reference data
	  void addRule (); //actions, based on relations

	  types::String getIndicators() const;
	  types::String getSignals() const;
	  types::String getRules() const;
	  bool isInitialized() const;

	  std::optional<Trade> ruleSignal (MarketData market_data) {
		  std::vector<Trade> trades;
		  const MarketData &market_data_;
		  for (const auto &rule : rules_){
			  auto trade = rule.ruleSignal(market_data_);
			  if (trade.has_value()) {
				  trades.push_back(std::move(trade));
			  }
		  }
		  //todo: plugin implementation, priority of trades is required
		  if (not trades.empty()) return *begin(trades);
		  else return std::nullopt;
	  }

  private:

	  types::String label_;
	  Account &account;
	  Ticker ticker;
	  std::vector<Rule> rules_;
	  bool is_initialized_ = false;

  };

}//!namespace
#endif //STRATEGY_TRADING_STRATEGY_H
