//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "object_type.h"
#include "data_processor.h"
#include "indicator.h"
#include "relations.h"

#include <type_traits>

#ifndef STRATEGY_TRADING_SIGNAL_H
#define STRATEGY_TRADING_SIGNAL_H

namespace algo {

  namespace signal_base {

	namespace signal_type {
	  struct BaseType {};
	  struct Comparison : BaseType{};
	  struct CrossOver : BaseType{};
	  struct Formula : BaseType{};
	  struct Pick : BaseType{};
	  struct Timestamp : BaseType{};
	  struct Threshold : BaseType{};
	}//!namespace

	using SignalTypeBase =
	std::variant<
			std::monostate,
			signal_type::Comparison,
			signal_type::CrossOver,
			signal_type::Formula,
			signal_type::Pick,
			signal_type::Timestamp,
			signal_type::Threshold
	>;

	struct SignalType final : public types::ObjectType<signal_base::SignalTypeBase> {};

	[[maybe_unused]]
	static types::String SignalTypeToString (const SignalType &type ) {
		if (type.TryAs<signal_type::Comparison>()) return "Comparison";
		else if (type.TryAs<signal_type::CrossOver>()) return "CrossOver";
		else if (type.TryAs<signal_type::Formula>()) return "Formula";
		else if (type.TryAs<signal_type::Pick>()) return "Pick";
		else if (type.TryAs<signal_type::Timestamp>()) return "Timestamp";
		else if (type.TryAs<signal_type::Threshold>()) return "Threshold";
		else return "";
	}

	[[maybe_unused]]
	static SignalType StringToSignalType (const types::String& type)   {
		if (type == "Comparison") return SignalType{signal_type::Comparison{}};
		else if (type == "CrossOver") return SignalType{signal_type::CrossOver{}};
		else if (type == "Formula") return SignalType{signal_type::Formula{}};
		else if (type == "Pick") return SignalType{signal_type::Pick{}};
		else if (type == "Timestamp") return SignalType{signal_type::Timestamp{}};
		else if (type == "Threshold") return SignalType{signal_type::Threshold{}};
		else return SignalType{};
	};

	using SignalData = boost::circular_buffer<int>;

  }//!namespace


  class Signal final {
  public:
	  Signal() = default;
	  Signal(const Signal&) = delete;

	  Signal(
			  const Ticker &ticker,
			  types::String signal_label,
			  types::String signal_type,
			  types::String relation,
			  std::vector<types::String> indicator_labels,
			  std::unique_ptr<Indicators> indicators
	  )
			  : ticker_(ticker)
			  , label_(std::move(signal_label))
			  , type_ (signal_base::StringToSignalType(signal_type))
			  , relation_(relations::RelationFromString(std::move(relation)))
			  , indicator_labels_(std::move(indicator_labels))
			  , indicators_(std::move(indicators))
	  {
		  signal_.resize(default_size_);
	  }

	  const signal_base::SignalData& getSignalData () {
		  ProcessSignal();
		  return signal_;
	  }
	  void updateSignal(const MarketData &market_data) {
		  indicators_->updateIndicators(market_data);
	  }

	  const types::String& getLabel() const {return label_;}
	  const Ticker& getTicker() const {return ticker_;}
  private:
	  Ticker ticker_;
	  types::String label_;
	  signal_base::SignalType type_;
	  relations::RelationType relation_;
	  std::vector<types::String> indicator_labels_;
	  std::unique_ptr<Indicators> indicators_;
	  signal_base::SignalData signal_;
	  size_t default_size_ = const_values::DEFAULT_INDICATOR_SIZE;

	  void ProcessSignal() {
		  //todo: make sure only new timestamps are updated

		  assert(indicator_labels_.size() == 2);
		  const auto &ind1 = indicators_->getIndicator(indicator_labels_[0]).getOutputValues();
		  const auto &ind2 = indicators_->getIndicator(indicator_labels_[1]).getOutputValues();

		  assert (ind1.size() == ind2.size());
		  int size = ind1.size();
		  if ((int)signal_.size() != size) signal_.resize(size);

		  for (int i = 0; i < size; ++i) {
			  bool rel = relations::RelationImpl( ind1[i], ind2[i], relation_);
			  if (i != 0) {
				  if (not rel and signal_[i-1] != -1) signal_.push_back(-1);
				  else signal_.push_back(0);
				  if (rel and signal_[i-1] != 1) signal_.push_back(1);
				  else signal_.push_back(0);
			  }
		  }
	  }

	  template <typename L, typename R, typename T>
	  std::common_type<L, R, T> ProcessSignalThreshold (L &&l, R &&r, T &&t) {return l - r - t;}

	  template <typename L, typename R>
	  std::common_type<L, R> ProcessSignalComparison(L &&l, R &&r) {return l - r;}
  };


  class Signals {
	  using Sig = std::shared_ptr<Signal>;
	  //todo: add thread safe map
	  using Signal_Label = types::String;
	  using ByLabel = types::SingleThreadedMap<Signal_Label, Sig>;
	  using ByTicker = types::MultiMap<Ticker, Sig>;
  public:
	  const ByLabel& getSignals () const {return by_label_;}
	  const Signal& getSignal (const types::String &label) const {
		  if (auto found = by_label_.find(label); found == by_label_.end()){
			  throw std::invalid_argument(EXCEPTION_MSG("Unknown signal label; "));
		  }
		  else return *found->second;
	  }
	  void updateSignals (const MarketData &market_data) {
		  if (const auto [first, last] = by_ticker_.equal_range(market_data.ticker); first != by_ticker_.end()) {
			  for (auto it = first, ite = last; it != ite; ++it) {
				  it->second->updateSignal(market_data);
			  }
		  }
	  }
	  void addSignal (Signal signal) {
		  const auto label = signal.getLabel();
		  const auto ticker = signal.getTicker();

		  if (auto found = by_label_.find(label); found != by_label_.end()){
			  throw std::invalid_argument(EXCEPTION_MSG("Signal already exists; "));
		  }
		  auto shared = std::make_shared<Signal>(std::move(signal));
		  by_label_[label] = shared;
		  by_ticker_.insert({ticker, shareSignal(label)});
	  }

	  void removeSignal (const types::String &label) {
	  	auto found = by_label_.find(label);
	  	if (found == by_label_.end()){
			  throw std::invalid_argument(EXCEPTION_MSG("Signal is not found; "));
		  }

		  //todo: very much dengerous - test this
		  if (const auto [first, last] = by_ticker_.equal_range(found->second->getTicker()); first != by_ticker_.end()) {
			  for (auto it = first, ite = last; it != ite;) {
				  if (it->second->getLabel() == label) by_ticker_.erase(it);
				  else ++it;
			  }
		  }
		  by_label_.erase(label);
	  }

	  [[nodiscard]] Sig shareSignal (const types::String &label) {
		  if (auto found = by_label_.find(label); found == by_label_.end())
			  throw std::invalid_argument(EXCEPTION_MSG("Unknown signal label; "));
		  else return std::shared_ptr<Signal>(&*found->second, [](auto *){/* empty deleter */});
	  }

  private:
	  ByLabel by_label_;
	  ByTicker by_ticker_;

  };


}//!namespace
#endif //STRATEGY_TRADING_SIGNAL_H
