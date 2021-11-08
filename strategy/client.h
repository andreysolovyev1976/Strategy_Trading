//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"

#ifndef STRATEGY_TRADING_CLIENT_H
#define STRATEGY_TRADING_CLIENT_H

namespace algo {

  struct Client {
  	types::String name;
  	types::BigUInt id;
  };

}//!namespace
#endif //STRATEGY_TRADING_CLIENT_H
