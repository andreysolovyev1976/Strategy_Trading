//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "rule.h"

#include <utility>

namespace algo {
  using namespace std::string_literals;

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
		  types::String rule_label,
		  const Ticker &ticker,
		  types::String quipuswap_trade_side,
//		  types::String rule_type,
		  types::String signal_label,
		  types::String signal_value,
//		  types::String position_side,
		  trade_base::OrderQuantity order_quantity,
//		  types::String trade_type,
//		  types::String order_type,
		  Signals* signals)
		  : label_(std::move(rule_label))
		  , trading_contract_(ticker, std::move(quipuswap_trade_side))
		  , rule_type_(rule_base::StringToRuleType("Entry"s))
		  , signal_label_(std::move(signal_label))
		  , signal_value_(signal_base::StringToSignalValue(signal_value))
		  , required_position_side_(StringToPositionSide("Neutral"s))
		  , order_quantity_(std::move(order_quantity))
		  , trade_type_(trade_base::StringToTradeType("Enter"s))
		  , order_type_(trade_base::StringToOrderType("Market"))
		  , signal_(signals->getSafePtr(signal_label_))
		  , indicator_labels_ ({signal_->getIndicatorsLabels().begin(), signal_->getIndicatorsLabels().end()})
  {}

  std::optional<Trade> Rule::ruleSignal () {
	  return ProcessSignal();
  }

  std::optional<Trade> Rule::ProcessSignal () {

	  const auto &signal_data = signal_->getSignalData ();

	  //todo: implement visitor
	  const auto type_ = signal_->getSignalType();
	  if (type_.TryAs<signal_base::signal_type::Comparison>()) {
		  if (not signal_data->empty() && std::prev(signal_data->end())->second == signal_value_){
			  return Trade (trading_contract_, order_quantity_, types::Value(0.005, 3));
		  }
		  else return std::nullopt;
	  }
	  else if (type_.TryAs<signal_base::signal_type::CrossOver>()) {}
	  else if (type_.TryAs<signal_base::signal_type::Formula>()) {}
	  else if (type_.TryAs<signal_base::signal_type::Pick>()) {}
	  else if (type_.TryAs<signal_base::signal_type::Timestamp>()) {}
	  else if (type_.TryAs<signal_base::signal_type::Threshold>()) {}

	  return std::nullopt;
  }

  const types::String& Rule::getLabel() const {
	  return label_;
  }

  const Ticker& Rule::getTicker() const {
	  return trading_contract_.ticker;
  }

  const std::set<types::String>& Rule::getIndicatorsLabels() const {
	  return indicator_labels_;
  }

  trading_contract_base::QiupuswapTradeSide Rule::getTradeSide() const {
  	return trading_contract_.quipuswap_trade_side;
  }

  void Rules::addRule (Rule rule) {
	  const auto label = rule.getLabel();

	  if (auto found = this->getByLabel()->find(label); found != this->getByLabel()->end()){
		  throw std::invalid_argument(EXCEPTION_MSG("Rule already exists; "));
	  }
	  auto new_rule = Ptr<Rule>(std::move(rule));
	  this->getByLabel()->insert({new_rule->getLabel(), std::move(new_rule)});
  }


  void Rules::removeRule (const types::String &label) {
	  auto found = this->getByLabel()->find(label);
	  if (found == this->getByLabel()->end()){
		  throw std::invalid_argument(EXCEPTION_MSG("Rule is not found; "));
	  }
	  else {
		  this->getByLabel()->erase(found);
	  }
  }

}//!namespace