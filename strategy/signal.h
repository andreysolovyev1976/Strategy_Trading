//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "object_type.h"
#include "data_processor.h"
#include "indicator.h"
#include "relations.h"

#include <type_traits>

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
	}

	template <typename I>
	using IsSignal = std::enable_if_t<
			std::disjunction_v<
					std::is_same<I, signal_type::Comparison>,
					std::is_same<I, signal_type::CrossOver>,
					std::is_same<I, signal_type::Formula>,
					std::is_same<I, signal_type::Pick>,
					std::is_same<I, signal_type::Timestamp>,
					std::is_same<I, signal_type::Threshold>
			>, bool>;

	template <typename S1, typename S2, IsSignal<S1> = true, IsSignal<S2> = true>
	bool constexpr IsSame (S1 lhs, S2 rhs) {
		return std::is_same_v<std::decay_t<decltype(lhs)>, std::decay_t<decltype(rhs)>>;
	}

	template <typename S1, typename S2, IsSignal<S1> = true, IsSignal<S2> = true>
	bool constexpr IsNotSame (S1 lhs, S2 rhs) {
		return not IsSame(lhs, rhs);
	}

	template<typename S>
	constexpr bool IsComparison (S) {
		constexpr bool ret = std::is_same_v<S, signal_type::Comparison>;
		return ret;
	}
	template<typename S>
	constexpr bool IsCrossOver (S) {
		constexpr bool ret = std::is_same_v<S, signal_type::CrossOver>;
		return ret;
	}
	template<typename S>
	constexpr bool IsFormula (S) {
		constexpr bool ret = std::is_same_v<S, signal_type::Formula>;
		return ret;
	}
	template<typename S>
	constexpr bool IsPick (S) {
		constexpr bool ret = std::is_same_v<S, signal_type::Pick>;
		return ret;
	}
	template<typename S>
	constexpr bool IsTimestamp (S) {
		constexpr bool ret = std::is_same_v<S, signal_type::Timestamp>;
		return ret;
	}
	template<typename S>
	constexpr bool IsThreshold (S) {
		constexpr bool ret = std::is_same_v<S, signal_type::Threshold>;
		return ret;
	}

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

  }//!namespace

  template <typename IndicatorData, typename IndicatorFunc = nullptr_t>
  class Signal final {
  	void Test (){}

	  template <typename MarketData>
	  types::Value operator()(MarketData market_data) {
  		return getSigValue(market_data);
	  }
  private:
	  signal_base::SignalType type_;
	  signal_base::signal_type::BaseType type__;
	  Indicator<IndicatorData, IndicatorFunc> indicator_;
	  relations::relation_type::BaseType relation_;

	  bool Relation (IndicatorData market_data) {
		  return relations::RelationImpl<IndicatorData, IndicatorData, decltype(relation_)>(indicator_, market_data);
	  }
	  types::String label_;

	  template <typename MarketData>
	  types::Value getSigValue(MarketData market_data) {
	  	if constexpr (signal_base::IsThreshold(type__)) {
	  		return indicator_ < market_data ? types::Value(-1) : indicator_ == market_data ? types::Value(0) : types::Value(1);
	  	}
	  }
  };

}//!namespace
#endif //STRATEGY_TRADING_SIGNAL_H
