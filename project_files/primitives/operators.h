//
// Created by Andrey Solovyev on 03/11/2021.
//

#pragma once

/// checking operators availability
/// https://stackoverflow.com/questions/6534041/how-to-check-whether-operator-exists

#include <utility>
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>

#ifndef STRATEGY_TRADING_OPERATORS_H
#define STRATEGY_TRADING_OPERATORS_H

namespace operators {
  template<class X, class Y, class Op>
  struct op_valid_impl
  {
	  template<class U, class L, class R>
	  static auto test(int) -> decltype(std::declval<U>()(std::declval<L>(), std::declval<R>()),
			  void(), std::true_type());

	  template<class U, class L, class R>
	  static auto test(...) -> std::false_type;

	  using type = decltype(test<Op, X, Y>(0));
  };

  template<class X, class Y, class Op> using op_valid = typename op_valid_impl<X, Y, Op>::type;

  namespace notstd {

	struct left_shift {

		template <class L, class R>
		constexpr auto operator()(L&& l, R&& r) const
		noexcept(noexcept(std::forward<L>(l) << std::forward<R>(r)))
		-> decltype(std::forward<L>(l) << std::forward<R>(r))
		{
			return std::forward<L>(l) << std::forward<R>(r);
		}
	};

	struct right_shift {

		template <class L, class R>
		constexpr auto operator()(L&& l, R&& r) const
		noexcept(noexcept(std::forward<L>(l) >> std::forward<R>(r)))
		-> decltype(std::forward<L>(l) >> std::forward<R>(r))
		{
			return std::forward<L>(l) >> std::forward<R>(r);
		}
	};

  }//!namespace

  template<class X, class Y> using has_equality = op_valid<X, Y, std::equal_to<>>;
  template<class X, class Y> using has_inequality = op_valid<X, Y, std::not_equal_to<>>;
  template<class X, class Y> using has_less_than = op_valid<X, Y, std::less<>>;
  template<class X, class Y> using has_less_equal = op_valid<X, Y, std::less_equal<>>;
  template<class X, class Y> using has_greater_than = op_valid<X, Y, std::greater<>>;
  template<class X, class Y> using has_greater_equal = op_valid<X, Y, std::greater_equal<>>;
  template<class X, class Y> using has_bit_xor = op_valid<X, Y, std::bit_xor<>>;
  template<class X, class Y> using has_bit_or = op_valid<X, Y, std::bit_or<>>;
  template<class X, class Y> using has_left_shift = op_valid<X, Y, notstd::left_shift>;
  template<class X, class Y> using has_right_shift = op_valid<X, Y, notstd::right_shift>;

  template <class X, class Y>using has_plus = op_valid<X, Y, std::plus<>>;
  template <class X, class Y>using has_minus = op_valid<X, Y, std::minus<>>;
  template <class X, class Y>using has_multiply = op_valid<X, Y, std::multiplies<>>;
  template <class X, class Y>using has_divides = op_valid<X, Y, std::divides<>>;
  template <class X, class Y>using has_modulus = op_valid<X, Y, std::modulus<>>;

  template <class X, class Y>using has_logical_and = op_valid<X, Y, std::logical_and<>>;
  template <class X, class Y>using has_logical_or = op_valid<X, Y, std::logical_or<>>;

}//!namespace

#endif //STRATEGY_TRADING_OPERATORS_H
