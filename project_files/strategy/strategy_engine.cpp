//
// Created by Andrey Solovyev on 28/03/2022.
//

#include "strategy_engine.h"
#include "os_pipe.h"
#include "dex_transaction.h"

#include <utility>
#include <iostream>

namespace algo {

  using namespace std::chrono_literals;
  using namespace algo::tezos::quipuswap::transaction;

  Engine::Engine (TgBot::Bot* b)
		  : bot (std::shared_ptr<TgBot::Bot>(b, [](auto*){ /* empty deleter */ }))
		  , data_processor_ptr(safe_ptr<DataProcessor>())
  {}

  void Engine::activateStrategy(const ActiveStrategy& strategy) {
	  const auto& [label, _] = strategy;
	  if (strategies.objectExists(label)) {
		  strategies.getSafePtr(label)->initializeTradingContracts(data_processor_ptr);
		  active_strategies.insert(strategy);
		  threads::Thread t (&Engine::runStrategy, this, strategy);
		  threads_engine.addThread(label, std::move(t)); //todo: should also be switched to pair <Label, Key>
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }
  void Engine::deactivateStrategy(const ActiveStrategy& strategy) {
	  const auto& [label, _] = strategy;
	  if (strategies.objectExists(label)) {
		  active_strategies.erase(strategy);
		  threads_engine.interruptThread(label); //todo: turn it on when change boost::thread to jthread
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }

  void Engine::getStrategies() const {}

  types::String Engine::implementTransaction (Trade trade, const TezosPrivateKey& key) const {
	  types::String result;
	  os::invokeCommandAndCaptureResult(
			  makeCommand(std::move(trade), key).c_str(),
			  result);
	  return result;
  }

  bool Engine::isStrategyActive(const ActiveStrategy& strategy) const {
	  return active_strategies.find(strategy) != active_strategies.end();
  }

  void Engine::runStrategy (const ActiveStrategy& strategy) {
	  const auto& [label, key] = strategy;
	  if (strategies.objectExists(label)) {
		  auto ptr_strategy = strategies.getSafePtr(label);
		  types::String result;
		  while (isStrategyActive(strategy)) {
			  auto generated_trades = ptr_strategy->processRules(data_processor_ptr);
			  result.clear();
			  if (generated_trades.has_value()) {
				  for (auto& trade : generated_trades.value()) {
					  result = implementTransaction(std::move(trade), key);
					  if (not result.empty()){
						  bot->getApi().sendMessage(ptr_strategy->getUserID(), result);
						  std::cerr << "Strategy Label - " << label << "\n " << result << '\n';
					  }
				  }
			  }
			  else {
				  std::cerr << "Strategy Label - " << label << ", no trades generated\n";
			  }
			  //todo: need to rationalize 10
			  // don't want to use boost here
			  std::this_thread::sleep_for(20s);
		  }
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }//!func
}//!namespace
