//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once


#include <string>
#include <cstdint>
#include <limits>
#include <boost/multiprecision/cpp_int.hpp>


#ifndef STRATEGY_TRADING_TYPES_DECLARATIONS_H
#define STRATEGY_TRADING_TYPES_DECLARATIONS_H

namespace types {

#ifdef _WIN32
  using String = std::wstring;
#else
  using String = std::string;
#endif

  using BigInt = boost::multiprecision::int128_t;
  static const BigInt BIGINT_MAX = std::numeric_limits<BigInt>::max();
  static const BigInt BIGINT_MIN = std::numeric_limits<BigInt>::min();
  using BigUInt = boost::multiprecision::uint128_t;
  static const BigUInt BIGUINT_MAX = std::numeric_limits<BigUInt>::max();
  static const BigUInt BIGUINT_MIN = std::numeric_limits<BigUInt>::min();
  using ShortUInt = uint16_t;


}//!namespace

#endif //STRATEGY_TRADING_TYPES_DECLARATIONS_H
