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
  static const size_t DEVELOPER_CHAT_ID = 442233888;
  static const size_t ALEX_CHAT = 1545167429;
//  static const types::String TG_BOT_TOKEN ("5062134652:AAGeSBm5sfECos5kFYDFUTWAid7q5mZgGs0"); //Alex's bot
  static const types::String TG_BOT_TOKEN ("5370993700:AAEmiacLIin2LD9-HjMa4xImaHqM82XjFWQ"); //Developers' test bot
  //TS_node file
  static const types::String TS_NODE_COMMAND ("ts-node ../setup_transacton_ts_script/");

  //Coinbase API
  static const types::String COINBASE_API_CURRENCIES ("https://api.exchange.coinbase.com/currencies");
  static const types::String COINBASE_API_PRODUCTS ("https://api.exchange.coinbase.com/products/");

  //tzkt API
  static const types::String TZKT_BASE_ADDRESS ("https://api.tzkt.io/v1/");

  //tezos dummy private key
  // const privateKey = "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz";
  // https://docs.quipuswap.com/smart-contracts/trusted-contracts


  static const std::vector<types::String> QUIPUSWAP_FACTORIES_ADDRESSES {
		  //FA_1_2
		  "KT1FWHLMk5tHbwuSsp31S4Jum4dTVmkXpfJw","KT1Lw8hCoaBrHeTeMXbqHPG4sS4K1xn7yKcD",
		  //FA_2
		  "KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ", "KT1SwH9P1Tx8a58Mm6qBExQFTcy2rwZyZiXS",
  };


}//!namespace

#endif //TEZOSBOT_CONST_VALUES_H