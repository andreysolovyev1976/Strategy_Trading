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


	types::String SignalValueToString (const SignalValue &value ) {
		if (value.TryAs<signal_value::True>()) return "True";
		else if (value.TryAs<signal_value::False>()) return "False";
		else if (value.TryAs<signal_value::Zero>()) return "Zero";
		else if (value.TryAs<signal_value::Value>()) return "Numeric";
		else return "";
	}

	SignalValue StringToSignalValue (const types::String& value) {
		if (value == "True") return SignalValue{signal_value::True{}};
		else if (value == "False") return SignalValue{signal_value::False{}};
		else if (value == "Zero") return SignalValue{signal_value::Zero{}};
		else if (value == "Numeric") return SignalValue{signal_value::Value{}}; //todo: parsing of argument is required here
		else return SignalValue{};
	}

	SignalValue BoolToSignalValue (bool relation) {
		if (relation == true) return SignalValue{signal_value::True{}};
		else return SignalValue{signal_value::False{}};
	}

	bool operator == (const SignalValue& lhs, const SignalValue& rhs) {
		if 		(lhs.TryAs<signal_value::True>() && rhs.TryAs<signal_value::True>()) return true;
		else if (lhs.TryAs<signal_value::False>() && rhs.TryAs<signal_value::False>()) return true;
		else if (lhs.TryAs<signal_value::Zero>() && rhs.TryAs<signal_value::Zero>()) return true;
		else if ((lhs.TryAs<signal_value::Value>() && rhs.TryAs<signal_value::Value>()) &&
				(lhs.As<signal_value::Value>().value == rhs.As<signal_value::Value>().value)) return true;

		else return false;
	}

  }//!namespace

  Signal::Signal(
		  types::String signal_label,
		  types::String signal_type,
		  types::String relation,
		  std::vector<types::String> indicator_labels,
		  Indicators* indicators
  )
		  : label_(std::move(signal_label))
		  , type_ (signal_base::StringToSignalType(signal_type))
		  , relation_(relations::RelationFromString(std::move(relation)))
		  , indicator_labels_(std::move(indicator_labels))
		  , indicators_(indicators)
		  , signal_ (types::makeMultiThreadedLimitedSizeMap<time_::Timestamp<time_::Seconds>, signal_base::SignalValue>())
  {}

  const signal_base::SignalData& Signal::getSignalData () {
	  ProcessSignal();
	  return signal_;
  }

  const types::String& Signal::getLabel() const {return label_;}
  const signal_base::SignalType& Signal::getSignalType() const {return type_;}
  const std::vector<types::String>& Signal::getIndicatorsLabels() {return indicator_labels_;}

  void Signal::ProcessSignal() {
	  //todo: make sure only new timestamps are updated

	  assert(indicator_labels_.size() == 2);

	  const auto &ind1 = indicators_->getObject(indicator_labels_[0]).getOutputValues();
	  const auto &ind2 = indicators_->getObject(indicator_labels_[1]).getOutputValues();

	  for (auto it1 = ind1->begin(), e1 = ind1->end(); it1 != e1; ++it1){
		  const auto &[tstamp1, quote1] = *it1;

		  if (auto found_at_second = ind2->Find(tstamp1); found_at_second != ind2->end()) {
			  const auto &[tstamp2, quote2] = *found_at_second;
			  bool rel = relations::RelationImpl(quote1, quote2, relation_);

			  if (auto found_at_signal = signal_->Find(tstamp1); found_at_signal == signal_->end()){
				  signal_->Insert({tstamp1, signal_base::BoolToSignalValue(rel)});

				  //todo: IMPORTANT!!! this doesn't work
/*
				  if (found_at_signal != signal_->begin()) {
					  auto &prev_singal_value = prev(found_at_signal)->second;
					  if (not rel && prev_singal_value != -1) signal_->Insert({tstamp1, -1});
					  else if (rel && prev_singal_value != 1) signal_->Insert({tstamp1, 1});
					  else signal_->Insert({tstamp1, 0});
				  }
				  else {
					  if (not rel) signal_->Insert({tstamp1, -1});
					  else signal_->Insert({tstamp1, 1});
				  }
*/
			  } else {
				  //todo: this must be addressed

//				  	throw std::invalid_argument (EXCEPTION_MSG("Attempt to update existing signal"));
			  }
		  }
		  else {
			  //todo: update indicators when there is no data
			  signal_->Insert({tstamp1, signal_base::SignalValue{}});
		  }
	  }
  }


  void Signals::addSignal (Signal signal) {
  	//todo: add check that indicator labels exist
	  const auto label = signal.getLabel();

	  if (auto found = this->getByLabel()->Find(label); found != this->getByLabel()->end()){
		  throw std::invalid_argument(EXCEPTION_MSG("Signal already exists; "));
	  }
	  auto new_signal = std::make_unique<Signal>(std::move(signal));
	  this->getByLabel()->Insert({new_signal->getLabel(), std::move(new_signal)});
}

  void Signals::removeSignal (const types::String &label) {
	  auto found = this->getByLabel()->Find(label);
	  if (found == this->getByLabel()->end()){
		  throw std::invalid_argument(EXCEPTION_MSG("Signal is not found; "));
	  }
	  else {
		  this->getByLabel()->Erase(found);
	  }
  }

}//!namespace