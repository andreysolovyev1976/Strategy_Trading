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

  static const size_t STRING_SIZE = 100; //bytes

  //Strategy, Indicator data size
  static const size_t DEFAULT_INDICATOR_SIZE = 30;

  //Quipuswap pair quote estimates
  static const double FEE_FACTOR = 0.003;
  static const size_t VALUE_FOR_QUOTE_DETERMINATION = 1'000'000;
  static const double FEE_VALUE_PRODUCT = 997'000;

  //INI file params
  static const types::String CONFIG_FILENAME ("../setup_user_data/bot_config.ini");

  static const types::String NO_NAME ("NO_NAME");
  // How often to check for price differences (e.g. every 5 minutes)
  static const int TIMEFRAME  = 60;


  //TG Bot
  static const size_t CHAT_ID = 442233888;
  static const types::String TG_BOT_TOKEN = "5062134652:AAG_9ecP_tQxSClfwYjPTsm5SYCqf_Y0_s8";

  //TS_node file
  static const types::String TS_NODE_COMMAND ("ts-node ../setup_transacton_ts_script/");

  //Coinbase API
  static const types::String COINBASE_API_CURRENCIES = "https://api.exchange.coinbase.com/currencies";
  static const types::String COINBASE_API_PRODUCTS = "https://api.exchange.coinbase.com/products/";

  //tzkt API
  static const types::String TZKT_BASE_ADDRESS = "https://api.tzkt.io/v1/";


}//!namespace

#endif //TEZOSBOT_CONST_VALUES_H