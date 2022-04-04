//
// Created by root on 9/17/21.
//
#pragma once

#include "types_declarations.h"
#include <cstddef>

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


  //INI file params
	static const types::String CONFIG_FILENAME ("../setup_user_data/bot_config.ini");
  static const types::String NO_NAME ("NO_NAME");
  static const int
		  PRICE_DIFF = 5,  		// Price differential ( in % difference relative to coinbase price)
		  TIMEFRAME  = 60,   	// How often to check for price differences (e.g. every 5 minutes)
		  TRADE_SIZE = 100,  	// Trade size (in tezos or whatever is easiest)
		  SLIPPAGE = 1;  		//Slippage: prevents the bot from making unfavorable trades (e.g. 1% slippage)


	//TG Bot
	static const size_t CHAT_ID = 442233888;
	static const types::String TG_BOT_TOKEN = "5062134652:AAG_9ecP_tQxSClfwYjPTsm5SYCqf_Y0_s8";

}//!namespace

#endif //TEZOSBOT_CONST_VALUES_H

/*
help - Short description
add_indicator - Adding an Indicator
remove_indicator - Removing an Indicator
get_indicators - List all the Indicators
add_signal - Adding a Signal
remove_signal - Removing a Signal
get_signals - List all the Signals
add_rule - Adding a Rule
remove_rule - Removing a Rule
get_rules - List all the Rules
add_strategy - Adding a Strategy
remove_strategy - Removing a Strategy
get_strategies - List all the Strategies
stop_strategy - Stop a Strategy from running
start_strategy - Start a Strategy running
add_contract - Adding a Contract
get_contracts - List all Current contracts available
 */