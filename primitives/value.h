//
// Created by Andrey Solovyev on 03/11/2021.
//

#pragma once

#include "exceptions.h"

#include <string>
#include <cstdint>
#include <cmath>
#include <iosfwd>
#include <stdexcept>
#include <optional>
#include <limits>

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
  using ShortUInt = uint16_t;

  template <typename Number>
  using IsInteger = std::enable_if_t<std::is_convertible_v<std::decay_t<Number>, BigInt>,bool>;
  template <typename Number>
  using IsNotInteger = std::enable_if_t<not std::is_convertible_v<std::decay_t<Number>, BigInt>,bool>;
//  template <typename Number>
//  using IsShortUnsignedInteger = std::enable_if_t<std::is_same_v<std::decay_t<Number>, uint16_t>, bool>;
  template <typename Number>
  using IsFloating = std::enable_if_t<std::is_floating_point_v<std::decay_t<Number>>, bool>;

  template <typename Str>
  using StringConvertible = std::enable_if_t<std::is_convertible_v<std::decay_t<Str>, String>, bool>;

  class Value {
  public:
	  Value() = default;

	  template <typename Str, StringConvertible<Str> = true>
	  explicit Value (String s) = delete;

	  explicit Value (BigInt &&i, BigInt &&d);
	  explicit Value (const BigInt &i, const BigInt &d);

	  template <typename Number, typename ShortUInt,
			  IsFloating<Number> = true>
//			  IsFloating<Number> = true, IsShortUnsignedInteger<ShortUInt> = true>
	  explicit Value (Number d, ShortUInt precision = 2, bool to_round = false)
			  : precision_ (precision)
			  , is_integer (false)
			  , is_rounded (to_round)
	  {
		  if (precision_ < 0 || precision_ > 20) throw std::invalid_argument(EXCEPTION_MSG("Precision is out of bounds; "));
		  double float_int;
		  double float_decimal = std::modf(d, &float_int);
		  integer.assign(float_int);
		  if (precision_ != 0){
			  decimal.assign(is_rounded
							 ? round(float_decimal * pow(10, precision_))
							 : floor(float_decimal * pow(10, precision_))
			  );
		  }
		  is_initialized = true;
	  }
	  template <typename Number, typename ShortUInt,
			  IsFloating<Number> = true>
//			  IsFloating<Number> = true, IsShortUnsignedInteger<ShortUInt> = true>
	  explicit Value (Number d, bool to_round, ShortUInt precision = 2)
			  : precision_ (precision)
			  , is_integer (false)
			  , is_rounded (to_round)
	  {
		  if (precision_ < 0 || precision_ > 20) throw std::invalid_argument(EXCEPTION_MSG("Precision is out of bounds; "));
		  double float_int;
		  double float_decimal = std::modf(d, &float_int);
		  integer.assign(float_int);
		  if (precision_ != 0){
			  decimal.assign(is_rounded
							 ? round(float_decimal * pow(10, precision_))
							 : floor(float_decimal * pow(10, precision_))
			  );
		  }
		  is_initialized = true;
	  }

	  template <typename Number, IsInteger<Number> = true>
	  explicit Value (Number i)
			  : is_integer (true)
			  , is_rounded (false)
	  {
		  integer.assign(i);
		  decimal.assign(0);
		  is_initialized = true;
	  }

	  String ToString () const;

	  constexpr bool IsInteger() const {return is_integer;}
	  constexpr bool IsDouble() const {return not is_integer;}
	  constexpr bool IsInitialized() const {return is_initialized;}
	  constexpr bool IsPositive() const {return Int() >= 0;}

	  inline BigUInt Int () const {return integer;}
	  inline BigUInt Decimal () const {return decimal;}
	  inline int Precision () const {return precision_;}
  private:
	  BigUInt integer = 0, decimal = 0;
	  int precision_;
	  bool is_initialized = false;
	  bool is_integer;
	  bool is_rounded;
  };


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
