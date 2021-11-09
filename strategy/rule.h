//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "data_processor.h"
#include "signal.h"
#include "position.h"
#include "trade.h"

#include <optional>

#ifndef STRATEGY_TRADING_RULE_H
#define STRATEGY_TRADING_RULE_H

namespace algo {

  namespace rule_base {

	namespace rule_type {
	  struct Entry {};
	  struct Exit {};
	  struct Risk {};
	  struct Order {};
	  struct Rebalance {};
	}//!namespace

	using RuleTypeBase =
	std::variant<
			std::monostate,
			rule_type::Entry,
			rule_type::Exit,
			rule_type::Risk,
			rule_type::Order,
			rule_type::Rebalance
	>;

	struct RuleType final : public types::ObjectType<rule_base::RuleTypeBase> {};
  }//!namespace


  class Rule {
  public:
  	Rule() = default;
	  Rule(
	  		types::String label,
	  		rule_base::RuleType type,
	  		types::String signal_label,
	  		int signal_value,
	  		std::unique_ptr<Signals> signals,
	  		PositionSide position_side,
	  		trade_base::OrderQuantity order_quantity,
	  		trade_base::OrderType order_type )
			  : label_(std::move(label))
			  , type_(std::move(type))
			  , signal_label_(std::move(signal_label))
			  , signal_value_(signal_value)
			  , signals_(std::move(signals))
			  , position_side_(std::move(position_side))
			  , order_quantity_(std::move(order_quantity))
			  , order_type_(std::move(order_type))
			  { }

	std::optional<Trade> ruleSignal (MarketData market_data) {
	  	const auto &market_data_ = market_data;
	  	signals_->updateSignals(market_data_);
	  	if (ProcessSignal()) return Trade;
	  }

  private:
  	types::String label_;
	rule_base::RuleType type_;
	types::String signal_label_;
	int signal_value_;
	std::unique_ptr<Signals> signals_;
	PositionSide position_side_;
	trade_base::OrderQuantity order_quantity_;
	trade_base::OrderType order_type_;

	bool ProcessSignal () {
		return true;
	}
  };


}//!namespace

#endif //STRATEGY_TRADING_RULE_H
