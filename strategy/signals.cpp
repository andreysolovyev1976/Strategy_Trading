//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "signals.h"

namespace algo {


  namespace signal_base {

	types::String SignalTypeToString (const SignalType &type ) {
		if (type.TryAs<signal_type::Comparison>()) return "Comparison";
		else if (type.TryAs<signal_type::CrossOver>()) return "CrossOver";
		else if (type.TryAs<signal_type::Formula>()) return "Formula";
		else if (type.TryAs<signal_type::Pick>()) return "Pick";
		else if (type.TryAs<signal_type::Timestamp>()) return "Timestamp";
		else if (type.TryAs<signal_type::Threshold>()) return "Threshold";
		else return "";
	}

	SignalType StringToSignalType (const types::String& type)   {
		if (type == "Comparison") return SignalType{signal_type::Comparison{}};
		else if (type == "CrossOver") return SignalType{signal_type::CrossOver{}};
		else if (type == "Formula") return SignalType{signal_type::Formula{}};
		else if (type == "Pick") return SignalType{signal_type::Pick{}};
		else if (type == "Timestamp") return SignalType{signal_type::Timestamp{}};
		else if (type == "Threshold") return SignalType{signal_type::Threshold{}};
		else return SignalType{};
	};


	types::String SignalValueToString (const SignalValue &type ) {
		if (type.TryAs<signal_value::True>()) return "True";
		else if (type.TryAs<signal_value::False>()) return "False";
		else if (type.TryAs<signal_value::Zero>()) return "Zero";
		else if (type.TryAs<signal_value::Value>()) return "Numeric";
		else return "";
	}

	SignalValue StringToSignalValue (const types::String& type) {
		if (type == "True") return SignalValue{signal_value::True{}};
		else if (type == "False") return SignalValue{signal_value::False{}};
		else if (type == "Zero") return SignalValue{signal_value::Zero{}};
		else if (type == "Numeric") return SignalValue{signal_value::Value{}}; //todo: parsing of argument is required here
		else return SignalValue{};
	};

  }//!namespace

  Signal::Signal()
  : signal_ (types::makeSingleThreadedLimitedSizeMap<time_::Timestamp<time_::Milliseconds>, int>())
  {}

  Signal::Signal(
		  const Ticker &ticker,
		  types::String signal_label,
		  types::String signal_type,
		  types::String relation,
		  std::vector<types::String> indicator_labels,
		  Indicators* indicators
  )
		  : ticker_(ticker)
		  , label_(std::move(signal_label))
		  , type_ (signal_base::StringToSignalType(signal_type))
		  , relation_(relations::RelationFromString(std::move(relation)))
		  , indicator_labels_(std::move(indicator_labels))
		  , indicators_(indicators)
		  , signal_ (types::makeSingleThreadedLimitedSizeMap<time_::Timestamp<time_::Milliseconds>, int>())
  {}

  const signal_base::SignalData& Signal::getSignalData () {
	  ProcessSignal();
	  return signal_;
  }
  void Signal::updateSignal([[maybe_unused]] const MarketData &market_data) {
//	  indicators_->updateIndicators(market_data);
  }

  const types::String& Signal::getLabel() const {return label_;}
  const Ticker& Signal::getTicker() const {return ticker_;}
  const signal_base::SignalType& Signal::getSignalType() const {return type_;}

  void Signal::ProcessSignal() {
	  //todo: make sure only new timestamps are updated

	  assert(indicator_labels_.size() == 2);

	  const auto &ind1 = indicators_->getObject(indicator_labels_[0]).getOutputValues();
	  const auto &ind2 = indicators_->getObject(indicator_labels_[1]).getOutputValues();

	  for (const auto &[tstamp1, quote1] : (*ind1)) {
		  if (auto found_at_second = ind2->Find(tstamp1); found_at_second != ind2->End()) {
			  const auto &[tstamp2, quote2] = *found_at_second;
			  bool rel = relations::RelationImpl(quote1, quote2, relation_);

			  if (auto found_at_signal = signal_->Find(tstamp1); found_at_signal == signal_->end()){
				  if (found_at_signal != signal_->begin()) {
					  auto &prev_singal_value = prev(found_at_signal)->second;
					  //todo: IMPORTANT!!! this doesn't work
					  if (not rel && prev_singal_value != -1) (*signal_).Insert({tstamp1, -1});
					  else if (rel && prev_singal_value != 1) (*signal_).Insert({tstamp1, 1});
					  else (*signal_).Insert({tstamp1, 0});
				  }
				  else {
					  if (not rel) (*signal_).Insert({tstamp1, -1});
					  else (*signal_).Insert({tstamp1, 1});
				  }
			  } else {
				  //todo: this must be addressed

//				  	throw std::invalid_argument (EXCEPTION_MSG("Attempt to update existing signal"));
			  }
		  }
		  else {
			  //todo: update indicators when there is no data
			  (*signal_).Insert({tstamp1, INT32_MIN});
		  }
	  }
  }


  void Signals::updateSignals (const MarketData &market_data) {
	  if (const auto [first, last] = by_ticker_->equal_range(market_data.ticker); first != by_ticker_->end()) {
		  for (auto it = first, ite = last; it != ite; ++it) {
			  it->second->updateSignal(market_data);
		  }
	  }
  }

  void Signals::addSignal (Signal signal) {
	  const auto label = signal.getLabel();

	  if (auto found = by_label_->Find(label); found != by_label_->end()){
		  throw std::invalid_argument(EXCEPTION_MSG("Signal already exists; "));
	  }
	  auto new_signal = std::make_unique<Signal>(std::move(signal));
	  by_label_->Insert({new_signal->getLabel(), std::move(new_signal)});

	  auto shared = shareObject(label);
	  by_ticker_->insert({shared->getTicker(), shared});
  }

  void Signals::removeSignal (const types::String &label) {
	  auto found = by_label_->Find(label);
	  if (found == by_label_->end()){
		  throw std::invalid_argument(EXCEPTION_MSG("Signal is not found; "));
	  }

	  //todo: very much dangerous - test this specific increment
	  if (const auto [first, last] = by_ticker_->equal_range(found->second->getTicker()); first != by_ticker_->end()) {
		  for (auto it = first, ite = last; it != ite;) {
			  if (it->second->getLabel() == label) by_ticker_->erase(it);
			  else ++it;
		  }
	  }
	  by_label_->Erase(label);
  }

}//!namespace