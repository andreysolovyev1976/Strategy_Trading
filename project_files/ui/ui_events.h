//
// Created by Andrey Solovyev on 26/03/2022.
//

#pragma once
#ifndef STRATEGY_TRADING_UI_EVENTS_H
#define STRATEGY_TRADING_UI_EVENTS_H

namespace user_interface {

  enum class Event : int {
	  begin,
	  help,

	  addIndicator,
	  addIndicator_Ticker,
	  addIndicator_TradeSide,
	  addIndicator_ModifierType,
	  addIndicator_ModifierValue,
	  removeIndicator,
	  getIndicators,

	  addSignal,
	  addSignal_Indicators,
	  addSignal_SignalType,
	  addSignal_Relation,
	  addSignal_SignalValue,
	  removeSignal,
	  getSignals,

	  addStrategy,
	  removeStrategy,
	  getStrategies,
	  stopStrategy,
	  startStrategy,

	  addRule,
	  addRule_TradingTicker,
	  addRule_TradeSide,
	  addRule_SignalLabel,
	  addRule_SignalValue,
	  addRule_OrderQuantity,
	  addRule_Slippage,
	  removeRule,
	  getRules,

	  addContract,
	  addContract_Label,
	  addContract_TickerQS,
	  addContract_TickerCB,
	  removeContract,
	  getContracts,

	  replaceTezosPrivateKey,
	  getTezosPrivateKey,

	  setup,
	  setup_selectMenuGroup,
	  setup_Indicators,
	  setup_Signals,
	  setup_Rules,
	  setup_Strategies,
	  setup_Contracts,
	  setup_TezosPrivateKey,

	  startOperations,
	  stopOperations,
	  startUI,
	  stopUI,
	  stopUI_Farewell,

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
