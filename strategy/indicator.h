//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "data_processor.h"
#include "quote.h"
#include <boost/circular_buffer.hpp>

#include <map>
#include <memory>
#include <ostream>

#ifndef STRATEGY_TRADING_INDICATOR_H
#define STRATEGY_TRADING_INDICATOR_H

namespace algo {

  using Container = boost::circular_buffer<Quote>;
  //todo: requires to get it callable for lambda
  //seems like is_callable is going to work just fine
  //or it can be done as std::function
  using ModifierFunc = Container (*) (Container);

  class Indicator final {
  public:
	  Indicator() = default;
//	  Indicator (const Indicator &other) = delete;

	  Indicator(const Ticker &ticker, types::String indicator_label, Container input_value, ModifierFunc modifier = nullptr);
	  Indicator(const Ticker &ticker, types::String indicator_label, ModifierFunc modifier = nullptr);

	  void updateIndicator (const MarketData &market_data);
	  const Container& getOutputValues() const;
	  const Container& getInputValues() const;
	  const types::String& getLabel () const;
	  const Ticker& getTicker() const;

	  bool Empty () const;
	  size_t Size() const;

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
	  using Ind = std::shared_ptr<Indicator>;
	  using IndicatorLabel = types::String;
	  //todo: add thread safe map
	  using ByLabel = types::SingleThreadedMap<IndicatorLabel, Ind>;
	  using ByTicker = types::MultiMap<Ticker, Ind>;
  public:
  	const ByLabel& getIndicators () const;
	  const Indicator& getIndicator (const types::String &label) const;
	  const Indicator& updateIndicators (const MarketData &market_data);
	  void addIndicator (Indicator indicator);
	  [[nodiscard]] Ind shareIndicator (const types::String &label);

  private:
  	ByLabel by_label_;
  	ByTicker by_ticker_;
  };

}//!namespace


#endif //STRATEGY_TRADING_INDICATOR_H
