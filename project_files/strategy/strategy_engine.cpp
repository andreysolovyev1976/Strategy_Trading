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
  : bot (b)
  , data_processor_ptr(std::make_shared<DataProcessor>())
  {}

  void Engine::activateStrategy(const types::String& label) {
	  if (auto found = strategies.getPtr(label); found) {
		  active_strategies.insert(label);
		  threads::Thread _ (&Engine::runStrategy, this, label);
		  threads_engine.addThread(label, std::move(_));
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }
  void Engine::deactivateStrategy(const types::String& label) {
	  if (auto found = strategies.getPtr(label); found) {
		  active_strategies.erase(label);
		  threads_engine.interruptThread(label); //todo: turn it on when change boost::thread to jthread
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }

  void Engine::getStrategies() const {}

  types::String Engine::implementTransaction (Trade trade) const {
	  types::String result;
	  os::invokeCommandAndCaptureResult(
			  makeCommand(std::move(trade)).c_str(),
			  result);
	  return result;
  }

  bool Engine::isStrategyActive(const types::String& label) const {
	  return active_strategies.find(label) != active_strategies.end();
  }

  void Engine::runStrategy (const types::String& label) {
	  if (auto strategy = strategies.getPtr(label); strategy) {
		  types::String result;
		  while (isStrategyActive(label)) {
			  auto generated_trades = strategy->processRules(data_processor_ptr);
			  result.clear();
			  if (generated_trades.has_value()) {
				  for (auto& trade : generated_trades.value()) {
					  result = implementTransaction(std::move(trade));
					  if (not result.empty()){
						  bot->getApi().sendMessage(strategy->getUserID(), result);
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
