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

  void Engine::run() {}
  void Engine::iterateOverStrategies(const std::set<types::String>& active_strategies) {
//	std::cout << time_::Timestamp<time_::Seconds>() << '\n';
	  while (true) {
		  for (const auto& label : active_strategies) {
			  if (auto strategy = strategies.getPtr(label); strategy) {
				  strategy->getMarketData();
				  auto trade = strategy->ruleSignal();
				  types::String result = "no signal for trade at strategy ";
				  if (trade.has_value()) {
					  result = implementTransaction(trade.value());
				  }
				  std::cerr << "Strategy Label - " << label << "\n " << result << '\n';
			  }
		  }
		  std::this_thread::sleep_for(10s);
	  }
  }

  void Engine::activateStrategy(const types::String& label, std::set<types::String>& active_strategies) {
	  if (auto found = strategies.getPtr(label); found) {
		  active_strategies.insert(label);
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }
  void Engine::deactivateStrategy(const types::String& label, std::set<types::String>& active_strategies) {
	  if (auto found = strategies.getPtr(label); found) {
		  active_strategies.erase(label);
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy Label is not found: " + label + "; "));
	  }
  }

  void Engine::getStrategies() const {}

  types::String Engine::implementTransaction ([[maybe_unused]] Trade trade) const {
	  types::String result;
	  os::invokeCommandAndCaptureResult(
			  makeCommand("KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT").c_str(),
			  result);
	  return result;
  }

}//!namespace
