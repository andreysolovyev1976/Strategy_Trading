//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "trade.h"
#include "indicator.h"
#include "signal.h"

#ifndef STRATEGY_TRADING_RULE_H
#define STRATEGY_TRADING_RULE_H

namespace algo {

  namespace rule_base {

	namespace rule_type {
	  struct Buy {};
	  struct Sell {};
	  struct DoNothing {};
	}//!namespace

	template <typename R>
	using IsRule = std::enable_if_t<
			std::disjunction_v<
					std::is_same<R, rule_type::Buy>,
					std::is_same<R, rule_type::Sell>,
					std::is_same<R, rule_type::DoNothing>
			>, bool>;

	template <typename R1, typename R2, IsRule<R1> = true, IsRule<R2> = true>
	bool constexpr IsSame (R1 lhs, R2 rhs) {
		return std::is_same_v<std::decay_t<decltype(lhs)>, std::decay_t<decltype(rhs)>>;
	}

	template <typename R1, typename R2, IsRule<R1> = true, IsRule<R2> = true>
	bool constexpr IsNotSame (R1 lhs, R2 rhs) {
		return not IsSame(lhs, rhs);
	}

	using RuleTypeBase =
	std::variant<
			std::monostate,
			rule_type::Buy,
			rule_type::Sell,
			rule_type::DoNothing
	>;

	struct RuleType final : public types::ObjectType<rule_base::RuleTypeBase> {};

  }//!namespace


  //ruletype
  //one of "risk","order","rebalance","exit","entry", see add.rule

  class Rule {
  public:


  private:
	  rule_base::RuleType type_;
  };


}//!namespace

#endif //STRATEGY_TRADING_RULE_H
