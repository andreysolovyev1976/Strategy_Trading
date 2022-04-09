//
// Created by Andrey Solovyev on 28/03/2022.
//

#pragma once

#include "tgbot/tgbot.h"

#include "base_objects_container.h"
#include "indicator.h"
#include "modifiers.h"
#include "signals.h"
#include "rule.h"
#include "strategy.h"
#include "trade.h"
#include "threads_engine.h"
#include "bot_config.h"
#include "data_processor.h"

#ifndef STRATEGY_TRADING_STRATEGY_ENGINE_H
#define STRATEGY_TRADING_STRATEGY_ENGINE_H

namespace algo {

  class Engine final {
  public:
	  explicit Engine (TgBot::Bot* b);
	  void activateStrategy(const types::String& label);
	  void deactivateStrategy(const types::String& label);
	  void getStrategies() const;

	  template <typename Object>
	  void addTradingObject (Object object);

	  template <typename Ptr>
	  Ptr* getPtr ();

  private:
	  TgBot::Bot* bot;
	  DataProcessorPtr data_processor_ptr;
	  Indicators indicators;
	  Modifiers<types::Value> modifiers; //todo make it available for other QuoteTypes
	  Signals signals;
	  Rules rules;
	  Strategies strategies;

	  threads::Engine<types::String> threads_engine;

	  std::set<types::String> active_strategies;
	  bool isStrategyActive(const types::String& label) const;
	  void runStrategy (const types::String& label);
  public:
	  types::String implementTransaction (Trade trade) const;
  };

  template <typename Object>
  void Engine::addTradingObject (Object object) {
	  if 	  constexpr(std::is_same_v<Object, Indicator>) indicators.addIndicator(std::move(object));
	  else if constexpr(std::is_same_v<Object, Modifier<types::Value>>) modifiers.addModifier(std::move(object));
	  else if constexpr(std::is_same_v<Object, Signal>) signals.addSignal(std::move(object));
	  else if constexpr(std::is_same_v<Object, Rule>) rules.addRule(std::move(object));
	  else if constexpr(std::is_same_v<Object, Strategy>) strategies.addStrategy(std::move(object));
	  else throw InvalidArgumentError(EXCEPTION_MSG("Some unknown type of Trading Object; "));
  }


  template <typename Ptr>
  Ptr* Engine::getPtr () {
	  if 	  constexpr(std::is_same_v<Ptr, Indicators>) return &indicators;
	  else if constexpr(std::is_same_v<Ptr, Modifiers<types::Value>>) return &modifiers;
	  else if constexpr(std::is_same_v<Ptr, Signals>) return &signals;
	  else if constexpr(std::is_same_v<Ptr, Rules>) return &rules;
	  else if constexpr(std::is_same_v<Ptr, Strategies>) return &strategies;
	  else return nullptr;
  }


}//!namespace


#endif //STRATEGY_TRADING_STRATEGY_ENGINE_H
