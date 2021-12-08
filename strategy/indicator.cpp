//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "indicator.h"

namespace algo {

  Indicator::Indicator(const Ticker &ticker, types::String indicator_label, MarketDataContainer input_value, const ModifierFunc &modifier)
		  : ticker_(ticker)
		  , label_(std::move(indicator_label))
		  , modifier(modifier)
		  , input_value_(std::move(input_value))
		  , output_value_(types::makeSingleThreadedLimitedSizeMap<K, V>())
  {
	  size_ = input_value_->Size();
	  this->ProcessIndicator();
  }

  Indicator::Indicator(const Ticker &ticker, types::String indicator_label, const ModifierFunc &modifier)
		  : ticker_(ticker)
		  , label_(std::move(indicator_label))
		  , modifier(modifier)
		  , input_value_(types::makeSingleThreadedLimitedSizeMap<K, V>())
		  , output_value_(types::makeSingleThreadedLimitedSizeMap<K, V>())
  {
	  this->ProcessIndicator();
  }

  Indicator::Indicator(const Ticker &ticker, types::String indicator_label, MarketDataContainer input_value)
		  : ticker_(ticker)
		  , label_(std::move(indicator_label))
		  , input_value_(std::move(input_value))
		  , output_value_(types::makeSingleThreadedLimitedSizeMap<K, V>())
  {
	  size_ = input_value_->Size();
	  this->ProcessIndicator();
  }

  Indicator::Indicator(const Ticker &ticker, types::String indicator_label)
		  : ticker_(ticker)
		  , label_(std::move(indicator_label))
		  , input_value_(types::makeSingleThreadedLimitedSizeMap<K, V>())
		  , output_value_(types::makeSingleThreadedLimitedSizeMap<K, V>())
  {
	  this->ProcessIndicator();
  }

  const MarketDataContainer& Indicator::getOutputValues() const {return output_value_;}
  const MarketDataContainer& Indicator::getInputValues() const {return input_value_;}
  const types::String& Indicator::getLabel () const {return label_;}
  const Ticker& Indicator::getTicker() const {return ticker_;}

  bool Indicator::Empty () const {
	  if (not modifier) return input_value_->Empty();
	  else return input_value_->Empty() && output_value_->Empty();
  }

  size_t Indicator::Size() const {return size_;}


  void Indicator::ProcessIndicator () {
	  if (input_value_->Empty()) return;
	  auto first_to_use = input_value_->Begin();
	  if (not output_value_->Empty()) {
		  auto last_updated = prev(output_value_->End());
		  //todo: check if next is ok here
		  if (auto found = input_value_->Find(last_updated->first); found != input_value_->End()) {
			  first_to_use = found;
		  }
	  }
	  if (modifier) {
		  for (auto it = first_to_use, ite = input_value_->End(); it != ite; ++it) {
			  (*output_value_).Insert({it->first, modifier(it->second)});
		  }
	  }
	  else {
		  for (auto it = first_to_use, ite = input_value_->End(); it != ite; ++it) {
			  (*output_value_).Insert({it->first, it->second});
		  }
	  }
  }


  std::ostream& operator<<(std::ostream& os, const Indicator &indicator) {
	  os << "label: " << indicator.getLabel() << '\n';
	  const auto &indicator_values = indicator.getOutputValues();
	  for (const auto &[tstamp, quote] : *indicator_values) {
		  os << tstamp << ": " << quote << '\n';
	  }
	  return os;
  }

  void Indicators::addIndicator (Indicator indicator) {
	  auto label = indicator.getLabel();
	  auto new_indicator = std::make_unique<Indicator>(std::move(indicator));
	  by_label_->Insert({new_indicator->getLabel(), std::move(new_indicator)});
	  auto placed_indicator = shareObject(label);
	  by_ticker_->insert({placed_indicator->getTicker(), placed_indicator});
  }

}//!namespace
