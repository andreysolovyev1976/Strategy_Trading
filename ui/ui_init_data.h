//
// Created by Andrey Solovyev on 26/03/2022.
//

#pragma once

#include "types_declarations.h"
#include <vector>

#ifndef STRATEGY_TRADING_INIT_DATA_H
#define STRATEGY_TRADING_INIT_DATA_H

namespace user_interface {

  struct InitData {
	  types::String label;
	  bool is_label_initialized;
	  virtual void clear() = 0;
	  virtual bool isInitialized() = 0;
	  virtual ~InitData() = default;
  };

  struct InitIndicator : public InitData {
	  algo::Ticker ticker;
	  bool is_ticker_initialized;
	  void clear() override
	  {
		  ticker = label = ""s;
		  is_ticker_initialized = is_label_initialized = false;
	  }
	  bool isInitialized() override
	  {
		  return is_label_initialized && is_ticker_initialized;
	  }

  };
  struct InitSignal : public InitData {
	  std::vector<types::String> indicator_labels;
	  const std::vector<types::String> signal_types = {
			  /// see signals.h for full list of types
			  /// can be automated as well once ready
			  "Comparison",
			  "CrossOver",
			  "Threshold",
	  };
	  types::String signal_type;
	  types::String relation; //using relations::relation_names as a universe
	  bool is_signal_type_initialized, is_relation_initialized, is_indicator_labels_initialized;

	  void clear() override
	  {
		  label = signal_type = relation = ""s;
		  indicator_labels.clear();

		  is_signal_type_initialized =
		  is_relation_initialized =
		  is_indicator_labels_initialized =
		  is_label_initialized =
				  false;
	  }
	  bool isInitialized() override
	  {
		  return
				  is_signal_type_initialized &&
						  is_relation_initialized &&
						  is_indicator_labels_initialized &&
						  is_label_initialized;
	  }

  };
  struct InitRule : public InitData {
	  void clear() override
	  {
		  label = ""s;
		  is_label_initialized = false;
	  }
	  bool isInitialized() override
	  {
		  return is_label_initialized;
	  }
  };

  struct InitStrategy : public InitData {
	  void clear() override
	  {
		  label = ""s;
		  is_label_initialized = false;
	  }
	  bool isInitialized() override
	  {
		  return is_label_initialized;
	  }
  };

}
#endif //STRATEGY_TRADING_INIT_DATA_H
