//
// Created by root on 9/17/21.
//
#pragma once

#ifndef TEZOSBOT_CONST_VALUES_H
#define TEZOSBOT_CONST_VALUES_H

namespace const_values {

  //Value default digit count after a point
  static const size_t DECIMALS_COUNT_DEFAULT = 6; //bytes



//print signalr::value
	static const size_t STRING_SIZE = 100; //bytes
	static const size_t VECTOR_STRING_SIZE = 2; //two strings in a vector, used for string split
	static const char DELIMITER = '/';  //maps, recursive keys syntax

	//Strategy, Indicator data size
  static const size_t DEFAULT_INDICATOR_SIZE = 30;

  //Quipuswap
  static const double FEE_FACTOR = 0.003;
  static const size_t VALUE_FOR_QUOTE_DETERMINATION = 1'000'000;
  static const double FEE_VALUE_PRODUCT = 997'000;


  static const size_t TIMEOUT_BETWEEN_INVOCATION = 5000; //milliseconds

}//!namespace

#endif //TEZOSBOT_CONST_VALUES_H
