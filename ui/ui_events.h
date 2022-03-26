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

	  addIndicator,
	  removeIndicator,
	  getIndicators,

	  addSignal,
	  removeSignal,
	  getSignals,

	  addStrategy,
	  removeStrategy,
	  addRule,
	  removeRule,
	  getRules,
	  stopStrategy,
	  startStrategy,

	  startOperations,
	  stopOperations,
	  startUI,
	  stopUI,
	  getContracts,
	  end,
  };

  Event nextEvent(Event current_event);
}

#endif //STRATEGY_TRADING_UI_EVENTS_H
