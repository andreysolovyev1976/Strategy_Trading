//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "rule.h"

namespace algo {

  namespace rule_base {

	types::String RuleTypeToString (const RuleType &type ) {
		if      (type.TryAs<rule_type::Entry>()) return "Entry";
		else if (type.TryAs<rule_type::Exit>()) return "Exit";
		else if (type.TryAs<rule_type::Risk>()) return "Risk";
		else if (type.TryAs<rule_type::Order>()) return "Order";
		else if (type.TryAs<rule_type::Rebalance>()) return "Rebalance";
		else return "";
	}

	RuleType StringToRuleType (const types::String& type) {
		if      (type == "Entry")return RuleType{rule_type::Entry{}};
		else if (type == "Exit") return RuleType{rule_type::Exit{}};
		else if (type == "Risk") return RuleType{rule_type::Risk{}};
		else if (type == "Order") return RuleType{rule_type::Order{}} ;
		else if (type == "Rebalance") return RuleType{rule_type::Rebalance{}};
		else return RuleType{};
	}

  }//!namespace


  Rule::Rule(
		  const Ticker &ticker,
		  types::String rule_label,
		  types::String rule_type,
		  types::String signal_label,
		  int signal_value,
		  Signal* signal,
		  types::String position_side,
		  trade_base::OrderQuantity order_quantity,
		  types::String trade_type,
		  types::String order_type,
		  Signals* signals)
		  : ticker_(ticker)
		  , label_(std::move(rule_label))
		  , rule_type_(rule_base::StringToRuleType(rule_type))
		  , signal_label_(std::move(signal_label))
		  , signal_value_(signal_value)
		  , signal_(std::move(signal))
		  , required_position_side_(StringToPositionSide(position_side))
		  , order_quantity_(std::move(order_quantity))
		  , trade_type_(trade_base::StringToTradeType(trade_type))
		  , order_type_(trade_base::StringToOrderType(order_type))
		  , signals_(signals)
  { }


  std::optional<Trade> Rule::ruleSignal () {
	  return ProcessSignal();
  }


  std::optional<Trade> Rule::ProcessSignal () {

	  const auto &value = signal_->getSignalData ();

	  //todo: implement visitor
	  const auto type_ = signal_->getSignalType();
	  if (type_.TryAs<signal_base::signal_type::Comparison>()) {
		  if (not value->Empty() &&
				  std::prev(value->End())->second == signal_value_){
			  Trade trade (ticker_,
					  order_quantity_,
					  required_position_side_,
					  trade_type_,
					  order_type_);

			  return trade;
		  }
		  else return std::nullopt;
	  }
	  else if (type_.TryAs<signal_base::signal_type::CrossOver>()) {}
	  else if (type_.TryAs<signal_base::signal_type::Formula>()) {}
	  else if (type_.TryAs<signal_base::signal_type::Pick>()) {}
	  else if (type_.TryAs<signal_base::signal_type::Timestamp>()) {}
	  else if (type_.TryAs<signal_base::signal_type::Threshold>()) {}

	  return Trade();
  }

  const types::String& Rule::getLabel() const {
  	return label_;
  }

  const Ticker& Rule::getTicker() const {
  	return ticker_;
  }

  void Rules::addRule (Rule rule) {
	  const auto label = rule.getLabel();

	  if (auto found = this->getByLabel()->Find(label); found != this->getByLabel()->end()){
		  throw std::invalid_argument(EXCEPTION_MSG("Rule already exists; "));
	  }
	  auto new_rule = std::make_unique<Rule>(std::move(rule));
	  this->getByLabel()->Insert({new_rule->getLabel(), std::move(new_rule)});

	  auto shared = shareObject(label);
	  this->getByTicker()->insert({shared->getTicker(), shared});
  }


  void Rules::removeRule (const types::String &label) {
	  auto found = this->getByLabel()->Find(label);
	  if (found == this->getByLabel()->end()){
		  throw std::invalid_argument(EXCEPTION_MSG("Rule is not found; "));
	  }
	  else {
		  this->getByLabel()->Erase(found);
	  }
  }

}//!namespace