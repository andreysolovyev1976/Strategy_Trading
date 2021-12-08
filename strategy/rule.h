//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "data_processor.h"
#include "signals.h"
#include "position.h"
#include "trade.h"
#include "base_objects_container.h"

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

	[[maybe_unused]]
	static types::String RuleTypeToString (const RuleType &type );

	[[maybe_unused]]
	static RuleType StringToRuleType (const types::String& type);

  }//!namespace


  class Rule final {
  public:
	  Rule() = default;
	  //todo: implement ctor as a variadic template
	  //todo: either to share related signals through special shared_ptr with no deleter
	  // or to share entire Signals using simple ptr
	  // same dilemma for Indicators
	  Rule(
			  const Ticker &ticker,
			  types::String rule_label,
			  types::String rule_type,
			  types::String signal_label,
			  int signal_value,
			  Signal *signal,
			  types::String position_side,
			  trade_base::OrderQuantity order_quantity,
			  types::String trade_type,
			  types::String order_type,
			  Signals* signals);

	  std::optional<Trade> ruleSignal (const MarketData &market_data);
	  std::optional<Trade> ruleSignal ();

	  const types::String& getLabel() const;
	  const Ticker& getTicker() const;
  private:
	  ///rule
	  Ticker ticker_;
	  types::String label_;
	  rule_base::RuleType rule_type_;
	  ///signal
	  types::String signal_label_;
	  int signal_value_;
	  Signal* signal_;
	  ///result of applying the rule over signal
	  PositionSide required_position_side_;
	  trade_base::OrderQuantity order_quantity_;
	  trade_base::TradeType trade_type_;
	  trade_base::OrderType order_type_;
	  [[maybe_unused]] Signals* signals_;


	  std::optional<Trade> ProcessSignal ();
  };


  class Rules final : public Objects<Rule> {
  public:
  	using Objects<Rule>::Objects;
	  void addRule (Rule rule);

  };


}//!namespace

#endif //STRATEGY_TRADING_RULE_H
