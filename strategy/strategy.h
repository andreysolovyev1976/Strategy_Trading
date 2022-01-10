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
#include <set>
#include <utility>

#ifndef STRATEGY_TRADING_STRATEGY_H
#define STRATEGY_TRADING_STRATEGY_H

namespace algo {

  namespace strategy_base {

  }//!namespace


  class Strategy {
  public:
	  Strategy () = default;
	  Strategy (types::String label, Account* account = nullptr)
			  : label_ (std::move(label))
			  , account_(account)
	  {}

	  void addIndicator(const Ticker & ticker, types::String indicator_label, MarketDataContainer input_value, ModifierFunc modifier) {
		  indicators_tickers_.emplace(ticker);
		  Indicator indicator (ticker, std::move(indicator_label), std::move(input_value), modifier);
		  indicators_.addIndicator(std::move(indicator));
	  }
	  void addIndicator(const Ticker & ticker, types::String indicator_label, MarketDataContainer input_value) {
		  indicators_tickers_.emplace(ticker);
		  Indicator indicator (ticker, std::move(indicator_label), std::move(input_value));
		  indicators_.addIndicator(std::move(indicator));
	  }
	  void addIndicator(const Ticker & ticker, types::String indicator_label, ModifierFunc modifier) {
		  indicators_tickers_.emplace(ticker);
		  Indicator indicator (ticker, std::move(indicator_label), modifier);
		  indicators_.addIndicator(std::move(indicator));
	  }
	  void addIndicator(const Ticker & ticker, types::String indicator_label) {
		  indicators_tickers_.emplace(ticker);
		  Indicator indicator (ticker, std::move(indicator_label));
		  indicators_.addIndicator(std::move(indicator));
	  }
	  void addSignal(
			  types::String signal_label,
			  types::String signal_type,
			  types::String relation,
			  std::vector<types::String> indicator_labels
	  ) {
		  Signal signal (
		  		std::move(signal_label),
		  		std::move(signal_type),
		  		std::move(relation),
		  		std::move(indicator_labels),
		  		&indicators_);
		  signals_.addSignal(std::move(signal));
	  }

	  void addRule (
	  		Ticker ticker,
			  types::String rule_label,
			  types::String rule_type,
			  const types::String& signal_label,
			  int signal_value,
			  types::String position_side,
			  trade_base::OrderQuantity order_quantity,
			  types::String trade_type,
			  types::String order_type )
	  {
		  Rule rule (
				  ticker,
				  std::move(rule_label),
				  std::move(rule_type),
				  signal_label, //todo: do I need this in ctor? Comes with ptr_signal
				  signal_value, //todo: do I need this in ctor? Comes with ptr_signal
				  signals_.shareObject(signal_label),
				  std::move(position_side),
				  std::move(order_quantity),
				  std::move(trade_type),
				  std::move(order_type),
				  &signals_);

		  rules_.addRule(std::move(rule));
	  }

	  types::String getIndicators() const;
	  types::String getSignals() const;
	  types::String getRules() const;
	  bool isInitialized() const {return not rules_.getByLabel()->Empty();}

	  void getMarketData(){
		  //todo: make it multithreading
		  //todo: it may be different types of Data and Duration for the some strategies
		  for (const auto &ticker : indicators_tickers_){
		  	auto market_data = DataProcessor_::getNewMarketData(ticker);
			  std::cout << "new data for ticker: " << market_data.first << "; " << market_data.second << '\n';
			  indicators_.updateIndicators(market_data);
		  }
		  std::cout << "==========\n";
	  }


	  std::optional<Trade> ruleSignal () {
		  std::vector<Trade> trades;

		  for (auto& rule : *rules_.getByLabel()) {
			  auto trade = rule.second->ruleSignal();
			  if (trade.has_value()) {
				  trades.push_back(std::move(trade.value()));
			  }
		  }

		  //todo: plugin implementation, priority of trades is required - select tickers!!!
		  if (not trades.empty()){
			  return *begin(trades);
		  }
		  else return std::nullopt;
	  }

  private:
	  types::String label_;
	  Account* account_;
	  Indicators indicators_;
	  std::set<Ticker> indicators_tickers_;
	  Signals signals_;
	  Rules rules_;
  };

}//!namespace
#endif //STRATEGY_TRADING_STRATEGY_H
