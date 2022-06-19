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
#include "data_processor.h"

#include <memory>
#include <mutex>

#ifndef STRATEGY_TRADING_STRATEGY_ENGINE_H
#define STRATEGY_TRADING_STRATEGY_ENGINE_H

namespace algo {

  struct ActiveStrategy {
	  using StrategyLabel = types::String;
	  using TezosPrivateKey = types::String;
	  StrategyLabel label;
	  TezosPrivateKey tpk;
	  DataProcessorPtr data_processor_ptr;
	  ActiveStrategy (StrategyLabel label, TezosPrivateKey tpk);
  };
  using ActiveStrategies = std::set<ActiveStrategy>;
  [[maybe_unused]] bool operator < (const ActiveStrategy& lhs, const ActiveStrategy& rhs);

  class Engine final {
  public:
	  explicit Engine (TgBot::Bot* b);
	  bool isStrategyActive(const ActiveStrategy& strategy) const;
	  void activateStrategy(const ActiveStrategy& strategy);
	  void deactivateStrategy(const ActiveStrategy& strategy);
	  void getStrategies() const;

	  template <typename Object>
	  void addTradingObject (Object object);

	  template <typename Ptr>
	  Ptr* getPtr ();

  private:
  	std::mutex mtx;
	  std::shared_ptr<TgBot::Bot> bot;
	  Indicators indicators;
	  Modifiers<types::Value> modifiers; //todo make it available for other QuoteTypes
	  Signals signals;
	  Rules rules;
	  Strategies strategies;

	  threads::Engine<types::String> threads_engine;

	  ActiveStrategies active_strategies;
	  void runStrategy (const ActiveStrategy& strategy);
  public:
	  types::String implementTransaction (Trade trade, const ActiveStrategy::TezosPrivateKey& key) const;
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
