//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "indicator.h"

namespace algo {
/*
  Indicator::Indicator(types::String indicator_label, const Ticker &ticker, types::String trade_side, MarketDataContainer input_value, ModifierFunc modifier)
		  : label_(std::move(indicator_label))
		  , trading_contract_ (ticker, trade_side)
		  , modifier(std::move(modifier))
		  , input_value_(std::move(input_value))
		  , output_value_(types::makeMultiThreadedLimitedSizeMap<K, V>())
  {
	  size_ = input_value_->Size();
	  this->ProcessIndicator();
  }
*/
  Indicator::Indicator(types::String indicator_label, const Ticker &ticker, types::String trade_side, ModifierFunc modifier)
		  : label_(std::move(indicator_label))
		  , trading_contract_ (ticker, trade_side)
		  , modifier(std::move(modifier))
		  , input_value_(types::makeMultiThreadedLimitedSizeMap<K, V>())
		  , output_value_(types::makeMultiThreadedLimitedSizeMap<K, V>())
  {
	  this->ProcessIndicator();
  }
/*
  Indicator::Indicator(types::String indicator_label, const Ticker &ticker, types::String trade_side, MarketDataContainer input_value)
		  : label_(std::move(indicator_label))
		  , trading_contract_ (ticker, trade_side)
		  , input_value_(std::move(input_value))
		  , output_value_(types::makeMultiThreadedLimitedSizeMap<K, V>())
  {
	  size_ = input_value_->Size();
	  this->ProcessIndicator();
  }
*/
  Indicator::Indicator(types::String indicator_label, const Ticker &ticker, types::String trade_side)
		  : label_(std::move(indicator_label))
		  , trading_contract_ (ticker, trade_side)
		  , input_value_(types::makeMultiThreadedLimitedSizeMap<K, V>())
		  , output_value_(types::makeMultiThreadedLimitedSizeMap<K, V>())
  {
	  this->ProcessIndicator();
  }

  const MarketDataContainer& Indicator::getOutputValues() const {return output_value_;}
  const MarketDataContainer& Indicator::getInputValues() const {return input_value_;}
  const types::String& Indicator::getLabel () const {return label_;}
  Ticker Indicator::getTicker () const {return trading_contract_.ticker;}
  const TradingContract& Indicator::getTradingContract () const {return trading_contract_;}
  TradingContract& Indicator::getTradingContract () {return trading_contract_;}

  bool Indicator::Empty () const {
	  if (not modifier) return input_value_->empty();
	  else return input_value_->empty() && output_value_->empty();
  }

  size_t Indicator::Size() const {return size_;}

  void Indicator::ProcessIndicator () {
	  if (input_value_->empty()) return;
	  auto first_to_use = input_value_->begin();
	  if (not output_value_->empty()) {
		  auto last_updated = prev(output_value_->end());
		  //todo: check if next is ok here
		  if (auto found = input_value_->find(last_updated->first); found != input_value_->end()) {
			  first_to_use = found;
		  }
	  }
	  if (modifier) {
		  for (auto it = first_to_use, ite = input_value_->end(); it != ite; ++it) {
			  it->second.value() = modifier(it->second.value());
			  output_value_->insert({it->first, it->second});
		  }
	  }
	  else {
		  for (auto it = first_to_use, ite = input_value_->end(); it != ite; ++it) {
			  output_value_->insert({it->first, it->second});
		  }
	  }
  }


  std::ostream& operator<<(std::ostream& os, const Indicator &indicator) {
	  os << "label: " << indicator.getLabel() << '\n';
	  const auto& indicator_data = indicator.getOutputValues();
	  for (auto it = indicator_data->begin(), e = indicator_data->end(); it != e; ++it){
		  const auto& [tstamp, quote] = *it;
		  os << tstamp << ": " << quote << '\n';
	  }
	  return os;
  }

  void Indicators::addIndicator (Indicator indicator) {
	  auto label = indicator.getLabel();
	  auto new_indicator = Ptr<Indicator>(std::move(indicator));
	  this->getByLabel()->insert({new_indicator->getLabel(), std::move(new_indicator)});
  }

}//!namespace
