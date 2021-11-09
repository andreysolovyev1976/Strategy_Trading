//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "indicator.h"

namespace algo {

  Indicator::Indicator(const Ticker &ticker, types::String indicator_label, Container input_value, ModifierFunc modifier)
		  : ticker_(ticker)
		  , label_(indicator_label)
		  , modifier(modifier)
		  , input_value_(std::move(input_value))
  {
	  size_ = input_value_.size();
	  this->ProcessIndicator();
  }

  Indicator::Indicator(const Ticker &ticker, types::String indicator_label, ModifierFunc modifier)
		  : ticker_(ticker)
		  , label_(indicator_label)
		  , modifier(modifier)
  {
	  input_value_.resize(size_);
	  this->ProcessIndicator();
  }

  void Indicator::updateIndicator (const MarketData &market_data) {
	  if (market_data.quote.timestamp > input_value_.back().timestamp){
		  input_value_.push_back(std::move(market_data.quote));
	  }
	  this->ProcessIndicator();
  }
  const Container& Indicator::getOutputValues() const {return output_value_;}
  const Container& Indicator::getInputValues() const {return input_value_;}
  const types::String& Indicator::getLabel () const {return label_;}
  const Ticker& Indicator::getTicker() const {return ticker_;}

  bool Indicator::Empty () const {
	  if (not modifier) return input_value_.empty();
	  else return input_value_.empty() && output_value_.empty();
  }

  size_t Indicator::Size() const {
	  return size_;
  }


  void Indicator::ProcessIndicator () {
	  if (modifier) output_value_ = modifier(input_value_);
	  else output_value_ = std::move(input_value_);
  }

  std::ostream& operator<<(std::ostream& os, const Indicator &indicator) {
	  os << "label: " << indicator.getLabel() << '\n';
	  const auto &indicator_values = indicator.getOutputValues();
	  for (const auto &value : indicator_values) {
		  os << value << '\n';
	  }
	  return os;
  }


  const Indicators::ByLabel& Indicators::getIndicators () const {
	  return by_label_;
  }

  const Indicator& Indicators::getIndicator (const types::String &label) const {
	  if (auto found = by_label_.find(label); found != by_label_.end())
		  throw std::invalid_argument(EXCEPTION_MSG("Unknown indicator label; "));
	  else return *found->second;
  }

  const Indicator& Indicators::updateIndicators (const MarketData &market_data) {
	  if (const auto [first, last] = by_ticker_.equal_range(market_data.ticker); first != by_ticker_.end()) {
		  for (auto it = first, ite = last; it != ite; ++it) {
			  it->second->updateIndicator(market_data);
		  }
	  }
  }

  void Indicators::addIndicator (Indicator indicator) {
	  by_label_[indicator.getLabel()] = std::make_shared<Indicator>(std::move(indicator));
  }
  Indicators::Ind Indicators::shareIndicator (const types::String &label) {
	  if (auto found = by_label_.find(label); found != by_label_.end())
		  throw std::invalid_argument(EXCEPTION_MSG("Unknown indicator label; "));
	  else return std::shared_ptr<Indicator>(&*found->second, [](auto *){/* empty deleter */});
  }

}//!namespace
