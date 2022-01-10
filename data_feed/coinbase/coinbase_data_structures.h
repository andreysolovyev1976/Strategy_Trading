//
// Created by Andrey Solovyev on 15/12/2021.
//

#pragma once

#include "types_declarations.h"

#ifndef STRATEGY_TRADING_COINBASE_DATA_STRUCTURES_H
#define STRATEGY_TRADING_COINBASE_DATA_STRUCTURES_H

namespace algo {
  namespace data {
	namespace coinbase {

	  static const auto input_fmt = []() -> types::String { return "%Y-%m-%dT%H:%M:%S"; }();

	  struct CurrencyData {
		  bool empty = true;
		  types::String
				  crypto_address_link,
				  crypto_transaction_link,
				  display_name,
				  type,
				  id,
				  max_precision,
				  min_size,
				  name,
				  status;
		  types::BigInt
				  max_withdrawal_amount,
				  min_withdrawal_amount;

	  };


	  struct TradeData {
		  bool empty = true;
		  types::String
				  ask,
				  bid,
				  price,
				  size,
				  time,
				  volume;
		  types::BigInt trade_id;
	  };

	}//!namespace
  }//!namespace
}//!namespace



#endif //STRATEGY_TRADING_COINBASE_DATA_STRUCTURES_H
