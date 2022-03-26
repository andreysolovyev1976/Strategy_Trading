//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "data_processor.h"
#include "base_objects_container.h"

#include "rule.h"
#include "trade.h"
#include "account.h"

#include <map>
#include <set>
#include <utility>

#ifndef STRATEGY_TRADING_STRATEGY_H
#define STRATEGY_TRADING_STRATEGY_H

namespace algo {

  class Strategy final {
  public:
	  Strategy () = default;
	  Strategy (types::String label, Account* account = nullptr);

	  void addIndicator(const Ticker & ticker, types::String indicator_label, MarketDataContainer input_value, ModifierFunc modifier);
	  void addIndicator(const Ticker & ticker, types::String indicator_label, MarketDataContainer input_value);
	  void addIndicator(const Ticker & ticker, types::String indicator_label, ModifierFunc modifier);
	  void addIndicator(const Ticker & ticker, types::String indicator_label);
	  void addSignal(
			  types::String signal_label,
			  types::String signal_type,
			  types::String relation,
			  std::vector<types::String> indicator_labels
	  );
	  void addRule (
	  		Ticker ticker,
			  types::String rule_label,
			  types::String rule_type,
			  const types::String& signal_label,
			  int signal_value,
			  types::String position_side,
			  trade_base::OrderQuantity order_quantity,
			  types::String trade_type,
			  types::String order_type );

	  const Indicators& getIndicators() const;
	  const Signals& getSignals() const;
	  const Rules& getRules() const;
	  bool isInitialized() const;
	  const types::String& getLabel () const;

	  void getMarketData();
	  std::optional<Trade> ruleSignal ();

  private:
	  types::String label_;
	  [[maybe_unused]] Account* account_;
	  Indicators indicators_;
	  std::set<Ticker> indicators_tickers_;
	  Signals signals_;
	  Rules rules_;
  };


  class Strategies final : public Objects <Strategy>{
  public:
	  using Objects<Strategy>::Objects;

	  ByTicker& getByTicker () = delete;
	  const ByTicker& getByTicker () const = delete;

	  void addStrategy (Strategy strategy);
	  void removeStrategy (const types::String &label);
  };

}//!namespace
#endif //STRATEGY_TRADING_STRATEGY_H
