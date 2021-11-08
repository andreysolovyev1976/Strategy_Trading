//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "value.h"
#include "trade.h"

#ifndef STRATEGY_TRADING_DATA_PROCESSOR_H
#define STRATEGY_TRADING_DATA_PROCESSOR_H

namespace algo {

  struct Data {

  };

  struct DataProcessor {
  	template <typename Input>
  	static types::Value toValue (Input &&input);
  };


}//!namespace

#endif //STRATEGY_TRADING_DATA_PROCESSOR_H
