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
	  Strategy (Ticker ticker, types::String label, Account* account)
			  : ticker_(std::move(ticker))
			  , label_ (std::move(label))
			  , account_(account)
	  {}

	  void addIndicator(Ticker ticker, types::String indicator_label, Container input_value, ModifierFunc modifier) {
		  Indicator indicator (ticker, indicator_label, input_value, modifier);
		  indicators_.addIndicator(std::move(indicator));
	  }
	  void addIndicator(Ticker ticker, types::String indicator_label, Container input_value) {
		  Indicator indicator (ticker, indicator_label, input_value);
		  indicators_.addIndicator(std::move(indicator));
	  }
	  void addIndicator(Ticker ticker, types::String indicator_label, ModifierFunc modifier) {
		  Indicator indicator (ticker, indicator_label, modifier);
		  indicators_.addIndicator(std::move(indicator));
	  }
	  void addIndicator(Ticker ticker, types::String indicator_label) {
		  Indicator indicator (ticker, indicator_label);
		  indicators_.addIndicator(std::move(indicator));
	  }
	  void addSignal(
			  types::String signal_label,
			  types::String signal_type,
			  types::String relation,
			  std::vector<types::String> indicator_labels
	  ) {
		  Signal signal (ticker_, signal_label, signal_type, relation, indicator_labels, &indicators_);
		  signals_.addSignal(std::move(signal));
	  }

	  void addRule (
			  types::String rule_label,
			  types::String rule_type,
			  types::String signal_label,
			  int signal_value,
			  types::String position_side,
			  trade_base::OrderQuantity order_quantity,
			  types::String trade_type,
			  types::String order_type )
	  {
		  Rule rule (
				  ticker_,
				  rule_label,
				  rule_type,
				  signal_label, //todo: do I need this in ctor? Comes with ptr_signal
				  signal_value, //todo: do I need this in ctor? Comes with ptr_signal
				  signals_.shareSignal(signal_label),
				  position_side,
				  order_quantity,
				  trade_type,
				  order_type,
				  &signals_);

		  rules_.emplace_back(std::move(rule));
	  }

	  types::String getIndicators() const;
	  types::String getSignals() const;
	  types::String getRules() const;
	  bool isInitialized() const {return not rules_.empty();}

	  std::optional<Trade> ruleSignal (const MarketData &market_data) {
		  std::vector<Trade> trades;

		  for (auto &rule : rules_){
			  auto trade = rule.ruleSignal(market_data);
			  if (trade.has_value()) {
				  trades.push_back(std::move(trade.value()));
			  }
		  }
		  //todo: plugin implementation, priority of trades is required
		  if (not trades.empty()) return *begin(trades);
		  else return std::nullopt;
	  }

  private:

	  Ticker ticker_;
	  types::String label_;
	  Account* account_;
	  std::vector<Rule> rules_;
	  Indicators indicators_;
	  Signals signals_;
  };

}//!namespace
#endif //STRATEGY_TRADING_STRATEGY_H
