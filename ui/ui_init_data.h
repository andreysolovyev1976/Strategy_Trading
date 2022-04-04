//
// Created by Andrey Solovyev on 26/03/2022.
//

#pragma once

#include "types_declarations.h"
#include "trading_contract.h"

#include <vector>

#ifndef STRATEGY_TRADING_INIT_DATA_H
#define STRATEGY_TRADING_INIT_DATA_H

namespace user_interface {

  using namespace std::literals;

  struct InitData {
	  types::String label;
	  bool is_label_initialized;
	  virtual void clear() = 0;
	  virtual bool isInitialized() = 0;
	  virtual ~InitData() = default;
  };

  struct InitIndicator : public InitData {
	  algo::Ticker ticker;
	  const std::vector<types::String> trade_sides = {
			  /// see signals.h for full list of types
			  /// can be automated as well once ready
			  "From Token to Tez",
			  "From Tez to Token",
	  };
	  types::String trade_side;
	  const std::vector<types::String> modifiers = {
			  "none",
			  "add",
			  "subtract",
			  "mutliply",
			  "divide",
	  };
	  types::String modifier;
	  types::Value modifier_value;

	  bool is_ticker_initialized, is_trade_side_initialized, is_modifier_initialized;
	  void clear() override
	  {
		  ticker =
		  label =
		  trade_side =
		  modifier =
				  ""s;
		  modifier_value = INT32_MIN; //todo: implement NaN analogy at Value class

		  is_ticker_initialized =
		  is_label_initialized =
		  is_trade_side_initialized =
		  is_modifier_initialized =
				  false;
	  }
	  bool isInitialized() override
	  {
		  return is_label_initialized &&
				  is_ticker_initialized &&
				  is_trade_side_initialized &&
				  is_modifier_initialized;
	  }
  };
  struct InitSignal : public InitData {
	  std::vector<types::String> indicator_labels;
	  const std::vector<types::String> signal_types = {
			  /// see signals.h for full list of types
			  /// can be automated as well once ready
			  "Comparison",
			  "CrossOver",
			  "Threshold",
	  };
	  types::String signal_type;
	  types::String relation; //using relations::relation_names as a universe
	  bool is_signal_type_initialized, is_relation_initialized, is_signal_value_initialized, is_indicator_labels_initialized;

	  void clear() override
	  {
		  label = signal_type = relation = ""s;
		  indicator_labels.clear();

		  is_signal_type_initialized =
		  is_relation_initialized =
		  is_indicator_labels_initialized =
		  is_label_initialized =
				  false;
	  }
	  bool isInitialized() override
	  {
		  return
				  is_label_initialized &&
						  is_signal_type_initialized &&
						  is_relation_initialized &&
						  is_indicator_labels_initialized;
	  }
  };

  struct InitRule : public InitData {
	  algo::Ticker ticker;
	  const std::vector<types::String> rule_types {"Entry", "Exit", "Risk", "Order", "Rebalance"};
	  types::String rule_type;
	  types::String signal_label;
	  const std::vector<types::String> signal_values {"True", "False", "Zero", "Value"};
	  types::String signal_value;

	  const std::vector<types::String> position_sides {"Neutral", "Long", "Short"};
	  types::String position_side;
	  int order_quantity;
	  const std::vector<types::String> trade_types {"Enter","Exit","StopLoss","TakeProfit","Chaining","Forced",};
	  types::String trade_type;
	  const std::vector<types::String> order_types {"Market", "Limited", "FillOrKill",};
	  types::String order_type;

	  const std::vector<types::String> trade_sides {"from Tez to Token", "from Token to Tez",};
	  types::String quipuswap_trade_side;

	  bool
			  is_ticker_initialized,
			  is_rule_type_initialized,
			  is_signal_label_initialized,
			  is_signal_value_initialized,
			  is_position_side_initialized,
			  is_order_quantity_initialized,
			  is_trade_type_initialized,
			  is_order_type_initialized,
			  is_quipuswap_trade_side_initialized;


	  void clear() override
	  {
		  label =
		  ticker =
		  rule_type =
		  signal_label =
		  signal_value =
		  position_side =
		  trade_type =
		  order_type =
		  quipuswap_trade_side =
				  ""s;

		  order_quantity =
				  0;

		  is_label_initialized =
		  is_ticker_initialized =
		  is_rule_type_initialized =
		  is_signal_label_initialized =
		  is_signal_value_initialized =
		  is_position_side_initialized =
		  is_order_quantity_initialized =
		  is_trade_type_initialized =
		  is_order_type_initialized =
		  is_quipuswap_trade_side_initialized =
				  false;
	  }
	  bool isInitialized() override
	  {
		  return
				  is_label_initialized &&
						  is_ticker_initialized &&
						  is_rule_type_initialized &&
						  is_signal_label_initialized &&
						  is_signal_value_initialized &&
						  is_position_side_initialized &&
						  is_order_quantity_initialized &&
						  is_trade_type_initialized &&
						  is_order_type_initialized &&
						  is_quipuswap_trade_side_initialized;
	  }
  };

  struct InitStrategy : public InitData {
	  std::vector<types::String> rules_labels;
	  bool is_rule_labels_initialized;

	  void clear() override {
		  label = ""s;
		  rules_labels.clear();
		  is_label_initialized = is_rule_labels_initialized = false;
	  }
	  bool isInitialized() override{
		  return is_label_initialized && is_rule_labels_initialized;
	  }
  };

  struct InitContract : public InitData {
	  bool is_ticker_qs_initialized, is_ticker_cb_initialized;
	  types::String ticker_qs;
	  types::String ticker_cb;

	  void clear() override {
		  label =
		  ticker_qs =
		  ticker_cb =
				  ""s;
		  is_label_initialized =
		  is_ticker_qs_initialized =
		  is_ticker_cb_initialized =
				  false;
	  }
	  bool isInitialized() override{
		  return is_label_initialized && (is_ticker_qs_initialized || is_ticker_cb_initialized);
	  }
  };


}
#endif //STRATEGY_TRADING_INIT_DATA_H
