//
// Created by Andrey Solovyev on 28/03/2022.
//

#include "strategy_engine.h"
#include "os_pipe.h"
#include "dex_transaction.h"

#include <utility>
#include <iostream>

namespace algo {

  ActiveStrategy::ActiveStrategy (StrategyLabel l, ActiveStrategy::TezosPrivateKey t)
		  : label(l)
		  , tpk(std::move(t))
		  , data_processor_ptr(Ptr<DataProcessor>())
  {}

  bool operator < (const ActiveStrategy& lhs, const ActiveStrategy& rhs) {
	  return lhs.label < rhs.label;
  }


  using namespace std::chrono_literals;
  using namespace algo::tezos::quipuswap::transaction;

  Engine::Engine (TgBot::Bot* b)
		  : bot (std::shared_ptr<TgBot::Bot>(b, [](auto*){ /* empty deleter */ }))
  {}

/*
  Engine::~Engine () {
	  for (auto curr = active_strategies->begin(), end = active_strategies->end(); curr != end; ++curr)
	  deactivateStrategy(*curr);
  }
*/

  void Engine::activateStrategy(ActiveStrategy&& strategy) {
	  std::lock_guard<std::mutex> lg(mtx3);
	  if (isStrategyActive(strategy)) return;
	  auto label = strategy.label;
	  if (strategies.objectExists(strategy.label)) {
		  const auto& [it, ok] = active_strategies.insert(std::move(strategy));
		  if (not ok) throw RuntimeError(EXCEPTION_MSG("unable to insert Active strategy to the according Set; "));
		  strategies.getSafePtr(it->label)->initializeTradingContracts(it->data_processor_ptr);
		  threads::Thread t (&ActiveStrategy::runStrategy, *it, std::cref(*this));
		  threads_engine.addThread(it->label, std::move(t)); //todo: should also be switched to pair <Label, Key>
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }
  void Engine::deactivateStrategy(const ActiveStrategy& strategy) {
	  std::lock_guard<std::mutex> lg(mtx2);
	  if (not isStrategyActive(strategy)) return;
	  const auto& [label, _, data_processor_ptr] = strategy;
	  if (strategies.objectExists(label)) {
		  threads_engine.interruptThread(label); //todo: turn it on when change boost::thread to jthread
		  active_strategies.erase(strategy);
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }

  void Engine::getStrategies() const {}


  bool Engine::isStrategyActive(const ActiveStrategy& strategy) const {
  	std::lock_guard<std::mutex> lg(mtx1);
	  return active_strategies.find(strategy) != active_strategies.end();
  }

  types::String ActiveStrategy::implementTransaction (Trade trade) const {
	  types::String result;
	  os::invokeCommandAndCaptureResult(
			  makeCommand(std::move(trade), tpk).c_str(),
			  result);
	  return result;
  }

  void ActiveStrategy::runStrategy (const Engine& engine) const {
	  if (engine.strategies.objectExists(label)) {
		  auto ptr_strategy = engine.strategies.getSafePtr(label);
		  types::String result;
		  while (engine.isStrategyActive(*this)) {
			  auto generated_trades = ptr_strategy->processRules(data_processor_ptr);
			  result.clear();
			  if (generated_trades.has_value()) {
				  for (auto& trade : generated_trades.value()) {
					  result = implementTransaction(std::move(trade));
					  if (not result.empty()){
						  engine.bot->getApi().sendMessage(ptr_strategy->getUserID(), result);
						  std::cout << "Strategy Label - " << label << "\n " << result << '\n';
					  }
				  }
			  }
			  else {
				  std::cout << "Strategy Label - " << label << ", no trades generated\n";
			  }
		  }
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }//!func
}//!namespace
