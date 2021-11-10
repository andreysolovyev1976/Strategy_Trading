//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "data_processor.h"
#include "quote.h"
#include <boost/circular_buffer.hpp>

#include <memory>
#include <string_view>
#include <ostream>
#include <functional>

#ifndef STRATEGY_TRADING_INDICATOR_H
#define STRATEGY_TRADING_INDICATOR_H

namespace algo {

  //todo: make it pointer to timestamp, no copy
  using Container = types::SingleThreadedMap<timestamp::Timestamp, Quote>;
  using ModifierFunc = std::function<Quote(const Quote&)>;

  class Indicator final {
  public:
	  Indicator() = default;

	  Indicator(const Ticker &ticker, types::String indicator_label, Container input_value, const ModifierFunc &modifier);
	  Indicator(const Ticker &ticker, types::String indicator_label, const ModifierFunc &modifier);
	  Indicator(const Ticker &ticker, types::String indicator_label, Container input_value);
	  Indicator(const Ticker &ticker, types::String indicator_label);

	  //todo: make a batch available, coming in a form of a vector
	  void updateIndicator (const MarketData &market_data);

	  [[nodiscard]] const Container& getOutputValues() const;
	  [[nodiscard]] const Container& getInputValues() const;
	  [[nodiscard]] const types::String& getLabel () const;
	  const Ticker& getTicker() const;

	  [[nodiscard]] bool Empty () const;
	  [[nodiscard]] size_t Size() const;

  private:
	  Ticker ticker_;
	  types::String label_;
	  ModifierFunc modifier;
	  Container input_value_, output_value_;
	  size_t size_ = const_values::DEFAULT_INDICATOR_SIZE;

	  void ProcessIndicator ();
  };


  std::ostream& operator<<(std::ostream& os, const Indicator &indicator);


  class Indicators {
	  using IndOwner = std::unique_ptr<Indicator>;
	  using IndPtr = Indicator*;
	  using ByLabel = types::SingleThreadedMap<std::string_view, IndOwner>;
	  using ByTicker = types::MultiMap<Ticker, IndPtr>;
  public:
	  [[nodiscard]] const ByLabel& getIndicators () const;

	  [[nodiscard]] const Indicator& getIndicator (const types::String &label) const;
	  void updateIndicators (const MarketData &market_data);
	  void addIndicator (Indicator indicator);
	  IndPtr shareIndicator (const types::String &label);

  private:
	  ByLabel by_label_; //owner of the indicators
	  ByTicker by_ticker_;
  };

}//!namespace


#endif //STRATEGY_TRADING_INDICATOR_H
