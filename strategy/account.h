//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "object_type.h"

#ifndef STRATEGY_TRADING_ACCOUNT_H
#define STRATEGY_TRADING_ACCOUNT_H

namespace algo {


  struct Account {

  	types::BigUInt id;
  	types::BigInt balance, equity, leverage;

  };

}//!namespace

#endif //STRATEGY_TRADING_ACCOUNT_H
