//
// Created by Andrey Solovyev on 28/03/2022.
//

#pragma once

#include "indicator.h"
#include "signals.h"
#include "rule.h"
#include "strategy.h"
#include "trade.h"
#include "threads_engine.h"
#include "bot_config.h"


#ifndef STRATEGY_TRADING_STRATEGY_ENGINE_H
#define STRATEGY_TRADING_STRATEGY_ENGINE_H

namespace algo {

  class Engine final {
  public:
	  void run();
	  void iterateOverStrategies(const std::set<types::String>& active_strategies);
	  void activateStrategy(const types::String& label, std::set<types::String>& active_strategies);
	  void deactivateStrategy(const types::String& label, std::set<types::String>& active_strategies);
	  void getStrategies() const;

	  template <typename Object>
	  void addTradingObject (Object object);

	  types::String implementTransaction (Trade trade) const;

	  template <typename Ptr>
	  Ptr* getPtr ();

  private:
	  Indicators indicators;
	  Signals signals;
	  Rules rules;
	  Strategies strategies;

	  threads::Engine threads_engine;

//	  config::RobotConfig robot_config;
  };

  template <typename Object>
  void Engine::addTradingObject (Object object) {
	  if 	  constexpr(std::is_same_v<Object, Indicator>) indicators.addIndicator(std::move(object));
	  else if constexpr(std::is_same_v<Object, Signal>) signals.addSignal(std::move(object));
	  else if constexpr(std::is_same_v<Object, Rule>) rules.addRule(std::move(object));
	  else if constexpr(std::is_same_v<Object, Strategy>) strategies.addStrategy(std::move(object));
	  else throw InvalidArgumentError(EXCEPTION_MSG("Some unknown type of Trading Object; "));
  }


  template <typename Ptr>
  Ptr* Engine::getPtr () {
	  if 	  constexpr(std::is_same_v<Ptr, Indicators>) return &indicators;
	  else if constexpr(std::is_same_v<Ptr, Signals>) return &signals;
	  else if constexpr(std::is_same_v<Ptr, Rules>) return &rules;
	  else if constexpr(std::is_same_v<Ptr, Strategies>) return &strategies;
	  else return nullptr;
  }


}//!namespace


#endif //STRATEGY_TRADING_STRATEGY_ENGINE_H
