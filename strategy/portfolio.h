//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "trading_contract.h"
#include "strategy.h"

#ifndef STRATEGY_TRADING_PORTFOLIO_H
#define STRATEGY_TRADING_PORTFOLIO_H

namespace algo {


  struct Portfolio {

  	void addStrategy (Strategy strategy);
  	bool strategyExists (const Ticker& tiker);
  	types::String getStrategy (const types::String &strategy_name);

  	const Position& getPosition();

  	types::SingleThreadedMap<Ticker, TradingContract> contracts;
  	types::SingleThreadedMap<Ticker, Strategy> strategies;
  };

}//!namespace

#endif //STRATEGY_TRADING_PORTFOLIO_H
