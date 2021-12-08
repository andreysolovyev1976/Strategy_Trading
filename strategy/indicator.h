//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "maps.h"
#include "data_processor.h"
#include "quote.h"
#include "base_objects_container.h"

#include <memory>
#include <string_view>
#include <ostream>
#include <functional>

#ifndef STRATEGY_TRADING_INDICATOR_H
#define STRATEGY_TRADING_INDICATOR_H

namespace algo {

  //todo: make it pointer to timestamp, no copy - check Indicator file
  using K = time_::Timestamp<time_::Milliseconds>;
  using V = Quote<types::Value, time_::Milliseconds>;
  using MarketDataContainer = types::SingleThreadedLimitedSizeMap<K, V>;
  using ModifierFunc = std::function<V(const V&)>;

  class Indicator final {
  public:
	  Indicator() = default;

	  Indicator(const Ticker &ticker, types::String indicator_label, MarketDataContainer input_value, const ModifierFunc &modifier);
	  Indicator(const Ticker &ticker, types::String indicator_label, const ModifierFunc &modifier);
	  Indicator(const Ticker &ticker, types::String indicator_label, MarketDataContainer input_value);
	  Indicator(const Ticker &ticker, types::String indicator_label);

	  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
	  void updateIndicator (const MarketData_<QuoteType, Duration> &market_data) {
		  input_value_->Insert({market_data.quote.timestamp, market_data.quote});
		  this->ProcessIndicator();
	  }

	  [[nodiscard]] const MarketDataContainer& getOutputValues() const;
	  [[nodiscard]] const MarketDataContainer& getInputValues() const;
	  [[nodiscard]] const types::String& getLabel () const;
	  const Ticker& getTicker() const;

	  [[nodiscard]] bool Empty () const;
	  [[nodiscard]] size_t Size() const;

  private:
	  Ticker ticker_;
	  types::String label_;
	  ModifierFunc modifier;
	  MarketDataContainer input_value_, output_value_;
	  size_t size_ = const_values::DEFAULT_INDICATOR_SIZE;

	  void ProcessIndicator ();
  };


  std::ostream& operator<<(std::ostream& os, const Indicator &indicator);


  class Indicators : public Objects<Indicator> {
  public:
  	using Objects<Indicator>::Objects;

	  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
	  void updateIndicators (const MarketData_<QuoteType, Duration> &market_data) {
		  if (const auto [first, last] = by_ticker_->equal_range(market_data.first); first != by_ticker_->end()) {
			  for (auto it = first, ite = last; it != ite; ++it) {
				  it->second->updateIndicator(market_data);
			  }
		  }
	  }

	  void addIndicator (Indicator indicator);
  };

}//!namespace


#endif //STRATEGY_TRADING_INDICATOR_H
