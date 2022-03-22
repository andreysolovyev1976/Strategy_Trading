//
// Created by Andrey Solovyev on 22/03/2022.
//

#pragma once

#include "user_defined_strings.h"
#include "user_defined_exceptions.h"
#include "os_pipe.h"

#ifndef STRATEGY_TRADING_DEX_TRANSACTION_H
#define STRATEGY_TRADING_DEX_TRANSACTION_H

namespace algo {
  namespace tezos {
	namespace quipuswap {
	  namespace transaction {

		types::String makeCommand (const types::String& contract_address);

	  }
	}
  }
}

#endif //STRATEGY_TRADING_DEX_TRANSACTION_H
