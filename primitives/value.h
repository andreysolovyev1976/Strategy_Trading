//
// Created by Andrey Solovyev on 03/11/2021.
//

#pragma once

#include <cstdint>

#include "user_defined_exceptions.h"
#include "user_defined_strings.h"
#include "const_values.h"
#if 0
#include "json.h" //circular reference
#endif

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#ifndef STRATEGY_TRADING_VALUE_H
#define STRATEGY_TRADING_VALUE_H

namespace types {

  using Int = int32_t;
  using UInt = uint64_t;

  using BigInt = boost::multiprecision::int128_t;
  static const BigInt BIGINT_MAX = std::numeric_limits<BigInt>::max();
  static const BigInt BIGINT_MIN = std::numeric_limits<BigInt>::min();
  using BigUInt = boost::multiprecision::uint128_t;
  static const BigUInt BIGUINT_MAX = std::numeric_limits<BigUInt>::max();
  static const BigUInt BIGUINT_MIN = std::numeric_limits<BigUInt>::min();

  template <typename Number>
  using IsInteger = std::enable_if_t<std::is_convertible_v<std::decay_t<Number>, BigInt>,bool>;
  template <typename Number>
  using IsNotInteger = std::enable_if_t<not std::is_convertible_v<std::decay_t<Number>, BigInt>,bool>;

  template <typename Number>
  bool constexpr IsNegative(Number n) {
  	return n < 0;
  }


  using Float = boost::multiprecision::cpp_dec_float_50;
  const static size_t MAX_DIGITS = std::numeric_limits<boost::multiprecision::cpp_dec_float_50>::max_digits10;

  template <typename Number>
  using IsFloating = std::enable_if_t<
		  std::disjunction_v<
				  std::is_floating_point<std::decay_t<Number>>,
				  std::is_same<Number, Float>
		  >
		  , bool>;

  template <typename Number>
  using IsNumber = std::enable_if_t<
		  std::disjunction_v<
				  std::is_convertible<std::decay_t<Number>, BigInt>,
				  IsFloating<std::decay_t<Number>>
		  >
		  , bool>;


  struct Value final {
	  Value() = default;
	  template <typename Number, IsFloating<Number> = true>
	  Value (Number d, size_t p = const_values::DECIMALS_COUNT_DEFAULT);
	  template <typename Number, IsInteger<Number> = true>
	  Value (Number i);

	  Value (const String &s, int p = const_values::DECIMALS_COUNT_DEFAULT);
#if 0
	  Value (const Json::Node& node, int p = 2);
#endif
	  //todo: somehow compiler required assignment operator - check again, violiting here Rule of Five
	  Value (const Value &other);
	  Value (Value &&other);
	  Value &operator = (const Value &other);
	  Value &operator = (Value &&other);
	  //~Value();

	  String ToString () const;

	  constexpr bool IsPositive() const;

	  Float number;
	  size_t precision;
	  bool is_integer;
  private:
#if 0
  	Float fromJsonInt (const Json::Node & node);
#endif
  };

  template <typename Number, IsFloating<Number>>
  Value::Value (Number d, size_t p)
		  : number(d)
		  , precision (p)
		  , is_integer (false)
  {
	  if (precision < 0 || precision > 50) throw std::invalid_argument(EXCEPTION_MSG("Precision is out of bounds; "));
  }
  template <typename Number, IsInteger<Number>>
  Value::Value (Number i)
		  : number(i)
		  , precision (0)
		  , is_integer (true)
  {}


  bool operator == (const Value& lhs, const Value& rhs);
  bool operator < (const Value& lhs, const Value& rhs);
  bool operator == (const Value& lhs, const Value& rhs);
  bool operator != (const Value& lhs, const Value& rhs);
  bool operator < (const Value& lhs, const Value& rhs);
  bool operator > (const Value& lhs, const Value& rhs);
  bool operator <= (const Value& lhs, const Value& rhs);
  bool operator >= (const Value& lhs, const Value& rhs);

  Value operator * (const Value &lhs, const Value &rhs);
  Value operator / (const Value &lhs, const Value &rhs);
  Value operator + (const Value &lhs, const Value &rhs);
  Value operator - (const Value &lhs, const Value &rhs);

  Value &operator += (Value &lhs, const Value &rhs);
  Value &operator -= (Value &lhs, const Value &rhs);
  Value &operator *= (Value &lhs, const Value &rhs);
  Value &operator /= (Value &lhs, const Value &rhs);

  std::ostream &operator << (std::ostream &os, const Value& r);
  std::istream &operator >> (std::istream &is, Value& r);

}//!namespace

#endif //STRATEGY_TRADING_VALUE_H
