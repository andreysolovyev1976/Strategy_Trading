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
  using K = time_::Timestamp<time_::Seconds>;
  using V = Quote<types::Value, time_::Seconds>;
  using MarketDataContainer = types::MultiThreadedLimitedSizeMap<K, V>;
  using ModifierFunc = std::function<types::Value(const types::Value&)>;

  class Indicator final {
  public:
//	  explicit Indicator(types::String indicator_label, const Ticker &ticker, types::String trade_side, MarketDataContainer input_value, ModifierFunc modifier);
	  explicit Indicator(types::String indicator_label, const Ticker &ticker, types::String trade_side, ModifierFunc modifier);
//	  explicit Indicator(types::String indicator_label, const Ticker &ticker, types::String trade_side, MarketDataContainer input_value);
	  explicit Indicator(types::String indicator_label, const Ticker &ticker, types::String trade_side);

	  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
	  void updateIndicator (const MarketData<QuoteType, Duration> &market_data) {
		  auto quote = market_data.second;
		  input_value_->insert({quote.timestamp, std::move(quote)});
		  this->ProcessIndicator();
	  }

	  [[nodiscard]] const MarketDataContainer& getOutputValues() const;
	  [[nodiscard]] const MarketDataContainer& getInputValues() const;
	  [[nodiscard]] const types::String& getLabel () const;
	  [[nodiscard]] Ticker getTicker () const;
	  [[nodiscard]] const TradingContract& getTradingContract () const;
	  [[nodiscard]] TradingContract& getTradingContract ();

	  [[nodiscard]] bool Empty () const;
	  [[nodiscard]] size_t Size() const;

  private:
	  types::String label_;
	  TradingContract trading_contract_;
	  ModifierFunc modifier;
	  MarketDataContainer input_value_, output_value_;
	  size_t size_ = const_values::DEFAULT_INDICATOR_SIZE;

	  void ProcessIndicator ();
  };


  std::ostream& operator<<(std::ostream& os, const Indicator &indicator);

  template<typename T>
  using IsIndicator = std::enable_if_t<std::is_same_v<T, Indicator>, bool>;


  class Indicators : public Objects<Indicator> {
  public:
	  using Objects<Indicator>::Objects;

	  template <typename QuoteType = types::Value, typename Duration = time_::Seconds>
	  void updateIndicator (Ptr<Indicator> ptr, const MarketData<QuoteType, Duration> &market_data) {
	  	ptr->updateIndicator(market_data);
	  }

	  template <typename QuoteType = types::Value, typename Duration = time_::Seconds>
	  void updateIndicator (const types::String& label, const MarketData<QuoteType, Duration> &market_data) {
	  	getSafePtr(label)->updateIndicator(market_data);
	  }

	  void addIndicator (Indicator indicator);
  };

}//!namespace


#endif //STRATEGY_TRADING_INDICATOR_H
