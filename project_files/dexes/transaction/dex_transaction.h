//
// Created by Andrey Solovyev on 22/03/2022.
//

#pragma once

#include "user_defined_strings.h"
#include "trade.h"

#ifndef STRATEGY_TRADING_DEX_TRANSACTION_H
#define STRATEGY_TRADING_DEX_TRANSACTION_H

namespace algo {
  namespace tezos {
	namespace quipuswap {
	  namespace transaction {

		types::String makeCommand (const Trade& trade);

	  }
	}
  }
}

#endif //STRATEGY_TRADING_DEX_TRANSACTION_H
