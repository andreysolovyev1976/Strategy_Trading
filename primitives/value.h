//
// Created by Andrey Solovyev on 03/11/2021.
//

#pragma once

#include "exceptions.h"
#include "strings.h"
#include "json.h"

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#ifndef STRATEGY_TRADING_VALUE_H
#define STRATEGY_TRADING_VALUE_H

namespace types {

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


  struct Value {
	  Value() = default;
	  template <typename Number, IsFloating<Number> = true>
	  Value (Number d, int p = 2);
	  template <typename Number, IsInteger<Number> = true>
	  Value (Number i);

	  Value (const String &s, int p = 2);
	  Value (const Json::Node& node, int p = 2);


	  String ToString () const;

	  constexpr bool IsPositive() const;

	  Float number;
	  int precision;
	  bool is_integer;
  private:
  	Float fromJsonInt (const Json::Node & node);

  };

  template <typename Number, IsFloating<Number>>
  Value::Value (Number d, int p)
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
