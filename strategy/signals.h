//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "object_type.h"
#include "data_processor.h"
#include "indicator.h"
#include "relations.h"

#include <memory>
#include <string_view>


#ifndef STRATEGY_TRADING_SIGNAL_H
#define STRATEGY_TRADING_SIGNAL_H

namespace algo {

  namespace signal_base {

	namespace signal_type {
	  struct BaseType {};
	  struct Comparison : BaseType{};
	  struct CrossOver : BaseType{};
	  struct Formula : BaseType{};
	  struct Pick : BaseType{};
	  struct Timestamp : BaseType{};
	  struct Threshold : BaseType{};
	}//!namespace

	using SignalTypeBase =
	std::variant<
			std::monostate,
			signal_type::Comparison,
			signal_type::CrossOver,
			signal_type::Formula,
			signal_type::Pick,
			signal_type::Timestamp,
			signal_type::Threshold
	>;

	struct SignalType final : public types::ObjectType<signal_base::SignalTypeBase> {};

	[[maybe_unused]]
	static types::String SignalTypeToString (const SignalType &type);
	[[maybe_unused]]
	static SignalType StringToSignalType (const types::String& type);

	namespace signal_value {
	  struct BaseType {};
	  struct True : BaseType{};
	  struct False : BaseType{};
	  struct Zero : BaseType{};
	  struct Value : BaseType{
//	  	Value() = default;
//	  	Value (types::Value v) : value(std::move(v)) {}
		  types::Value value;
	  };
	}//!namespace

	using SignalValueBase =
	std::variant<
			std::monostate,
			signal_value::True,
			signal_value::False,
			signal_value::Zero,
			signal_value::Value
	>;

	struct SignalValue final : public types::ObjectType<SignalValueBase> {
//		SignalValue (int i) {
//			if (i == -1) this->emplace<signal_value::False>();
//			else if (i == 1) this->emplace<signal_value::True>();
//			else if (i == 0) this->emplace<signal_value::Zero>();
//			else this->emplace<signal_value::Value>(types::Value(i));
//		}
	};

	[[maybe_unused]]
	static types::String SignalValueToString (const SignalValue &type );
	[[maybe_unused]]
	static SignalValue StringToSignalValue (const types::String& type);

	using SignalData = types::SingleThreadedLimitedSizeMap<timestamp::Timestamp<timestamp::Ms>, int>;

  }//!namespace


  class Signal final {
  public:
	  Signal();

	  Signal(
			  const Ticker &ticker,
			  types::String signal_label,
			  types::String signal_type,
			  types::String relation,
			  std::vector<types::String> indicator_labels,
			  Indicators* indicators
	  );

	  const signal_base::SignalData& getSignalData ();
	  void updateSignal(const MarketData &market_data);
	  const types::String& getLabel() const;
	  const Ticker& getTicker() const;
	  const signal_base::SignalType& getSignalType() const;

  private:
	  ///signal
	  Ticker ticker_;
	  types::String label_;
	  signal_base::SignalType type_;
	  relations::RelationType relation_;
	  ///indicator
	  std::vector<types::String> indicator_labels_;
	  Indicators* indicators_;
	  ///result of calculating the signal over indicator
	  signal_base::SignalData signal_;

	  void ProcessSignal();
	  template <typename L, typename R, typename T>
	  std::common_type<L, R, T> ProcessSignalThreshold (L &&l, R &&r, T &&t) {return l - r - t;}

	  template <typename L, typename R>
	  std::common_type<L, R> ProcessSignalComparison(L &&l, R &&r) {return l - r;}
  };


  class Signals {
	  using SigOwner = std::unique_ptr<Signal>;
	  using SigPtr = Signal*;
	  //todo: add thread safe map
	  using ByLabel = types::SingleThreadedLimitedSizeMap<std::string_view, SigOwner>;
	  using ByTicker = types::SingleThreadedMultiMap<std::string_view, SigPtr>;
  public:
	  Signals();
	  [[nodiscard]] const ByLabel& getSignals () const;
	  [[nodiscard]] const Signal& getSignal (const types::String &label) const;
	  void updateSignals (const MarketData &market_data);
	  void addSignal (Signal signal);

	  void removeSignal (const types::String &label);

	  [[nodiscard]] SigPtr shareSignal (const types::String &label);

  private:
	  ///owner of the data
	  ByLabel by_label_;
	  ///shallow copies
	  ByTicker by_ticker_;
  };


}//!namespace
#endif //STRATEGY_TRADING_SIGNAL_H
