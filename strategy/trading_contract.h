//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"

#ifndef STRATEGY_TRADING_TRADING_CONTRACT_H
#define STRATEGY_TRADING_TRADING_CONTRACT_H

namespace algo {

  struct TradingContract {
  	Ticker ticker;
  	types::String id_at_chain;
  };


}//!namespace

#endif //STRATEGY_TRADING_TRADING_CONTRACT_H
