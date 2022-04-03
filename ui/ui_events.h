//
// Created by Andrey Solovyev on 26/03/2022.
//

#pragma once
#ifndef STRATEGY_TRADING_UI_EVENTS_H
#define STRATEGY_TRADING_UI_EVENTS_H

namespace user_interface {

  enum class Event : int {
	  begin,

	  initRobot,

	  help,

	  addIndicator,
	  addIndicator_Ticker,
	  addIndicator_TradeSide,
	  removeIndicator,
	  getIndicators,

	  addSignal,
	  removeSignal,
	  getSignals,

	  addStrategy,
	  removeStrategy,

	  addRule,
	  addRule_TradingTicker,
	  addRule_TradeSide,
	  addRule_SignalLabel,
	  addRule_SignalValue,
	  addRule_PairTrade,
	  removeRule,
	  getRules,

	  stopStrategy,
	  startStrategy,

	  startOperations,
	  stopOperations,
	  startUI,
	  stopUI,

	  addContract,
	  addContract_Label,
	  addContract_TickerQS,
	  addContract_TickerCB,
	  getContracts,

	  end,
  };

  inline
  Event nextEvent(Event current_event) {
	  if (current_event==Event::end) return current_event;
	  int curr = static_cast<int>(current_event);
	  return static_cast<Event>(++curr);
  }
}

#endif //STRATEGY_TRADING_UI_EVENTS_H
