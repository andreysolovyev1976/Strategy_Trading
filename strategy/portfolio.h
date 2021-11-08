//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "trade.h"
#include "strategy.h"

#include <unordered_map>

#ifndef STRATEGY_TRADING_PORTFOLIO_H
#define STRATEGY_TRADING_PORTFOLIO_H

namespace algo {

  struct Position {
  };

  struct Portfolio {

  	void addStrategy (Strategy strategy);
  	bool strategyExists (const trade_base::Ticker& tiker);
  	types::String getStrategy (const types::String &strategy_name);

  	const Position& getPosition();

  	std::unordered_map<trade_base::Ticker, trade_base::TradingContract> contracts;
  	std::unordered_map<trade_base::Ticker, trade_base::TradingContract> strategies;
  };

}//!namespace

#endif //STRATEGY_TRADING_PORTFOLIO_H
