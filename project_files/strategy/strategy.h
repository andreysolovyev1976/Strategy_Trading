//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "data_processor.h"
#include "base_objects_container.h"

#include "rule.h"
#include "trade.h"
#include "account.h"
#include "trading_contract.h"

#include <map>
#include <set>
#include <utility>

#ifndef STRATEGY_TRADING_STRATEGY_H
#define STRATEGY_TRADING_STRATEGY_H

namespace algo {

  class Strategy final {
  public:
	  Strategy () = default;
	  Strategy (types::String label,
			  Indicators* indicators,
			  Signals* signals,
			  Rules* rules,
			  int64_t user_id,
			  Account* account = nullptr
	  );

	  void addRule (const types::String& label);
	  void initializeTradingContracts(DataProcessorPtr data_processor_ptr);
	  bool isInitialized() const;
	  const types::String& getLabel () const;
	  int64_t getUserID() const;

	  std::optional<std::vector<Trade>> processRules (DataProcessorPtr data_processor_ptr);

  private:
	  types::String label_;
	  std::set<types::String> rule_labels_;
	  std::set<types::String> indicator_labels_;

	  [[maybe_unused]] Indicators* indicators_;
	  [[maybe_unused]] Signals* signals_;
	  [[maybe_unused]] Rules* rules_;
	  [[maybe_unused]] int64_t user_id_;
	  [[maybe_unused]] Account* account_;

	  void getMarketData(DataProcessorPtr data_processor_ptr);
  };

  template<typename T>
  using IsStrategy = std::enable_if_t<std::is_same_v<T, Strategy>, bool>;


  class Strategies final : public Objects <Strategy>{
  public:
	  using Objects<Strategy>::Objects;

	  void addStrategy (Strategy strategy);
	  void removeStrategy (const types::String &label);
  };

}//!namespace
#endif //STRATEGY_TRADING_STRATEGY_H
