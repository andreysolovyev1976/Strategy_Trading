//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "object_type.h"
#include "data_processor.h"
#include "indicator.h"
#include "relations.h"
#include "base_objects_container.h"

#include <memory>
#include <set>
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
	types::String SignalTypeToString (const SignalType &type);
	[[maybe_unused]]
	SignalType StringToSignalType (const types::String& type);

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
	types::String SignalValueToString (const SignalValue &value );
	[[maybe_unused]]
	SignalValue StringToSignalValue (const types::String& value);
	[[maybe_unused]]
	SignalValue BoolToSignalValue (bool relation);

	bool operator == (const SignalValue& lhs, const SignalValue& rhs);

	using SignalData = types::MultiThreadedLimitedSizeMap<time_::Timestamp<time_::Seconds>, signal_base::SignalValue>;

  }//!namespace


  class Signal final {
  public:
	  explicit Signal(
			  types::String signal_label,
			  types::String signal_type,
			  types::String relation,
			  std::vector<types::String> indicator_labels,
			  Indicators* indicators
	  );

	  const signal_base::SignalData& getSignalData ();
	  const types::String& getLabel() const;
	  const signal_base::SignalType& getSignalType() const;
	  const std::vector<types::String>& getIndicatorsLabels();

  private:
	  ///signal
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


  template<typename T>
  using IsSignal = std::enable_if_t<std::is_same_v<T, Signal>, bool>;


  class Signals final : public Objects <Signal>{
  public:
	  using Objects<Signal>::Objects;

	  void addSignal (Signal signal);
	  void removeSignal (const types::String &label);
  };


}//!namespace
#endif //STRATEGY_TRADING_SIGNAL_H
