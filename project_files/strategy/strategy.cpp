//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "utils.h"

#include "strategy.h"
#include "user_defined_exceptions.h"

namespace algo {

  Strategy::Strategy (types::String label,
		  Indicators* indicators,
		  Signals* signals,
		  Rules* rules,
		  int64_t user_id,
		  Account* account)
		  : label_ (std::move(label))
		  , indicators_ (indicators)
		  , signals_(signals)
		  , rules_(rules)
		  , user_id_(user_id)
		  , account_(account)
  {}//!ctor

  void Strategy::addRule (const types::String& label) {
	  if (rule_labels_.count(label)) throw InvalidArgumentError(EXCEPTION_MSG("Rule Label already exists in this Strategy; "));
	  if (auto found = rules_->getByLabel()->find(label); found != rules_->getByLabel()->end()) {
		  rule_labels_.insert(label);
		  for (const auto& i: found->second->getIndicatorsLabels()) {
			  indicator_labels_.insert(i);
		  }
	  }
	  else{
		  throw RuntimeError(EXCEPTION_MSG("Rule Label is not found in the Rules; "));
	  }

  }
  bool Strategy::isInitialized() const {
	  return (not rule_labels_.empty());
  }

  const types::String& Strategy::getLabel () const {return label_;}
  int64_t Strategy::getUserID() const {return user_id_;}

  std::optional<std::vector<Trade>> Strategy::processRules (DataProcessorPtr data_processor_ptr) {
	  if (not isInitialized()) {
		  throw RuntimeError(EXCEPTION_MSG("No Rules selected; "));
	  }
	  getMarketData(data_processor_ptr);
	  std::vector<Trade> trades;
	  trades.reserve(rule_labels_.size());

	  for (const auto& r : rule_labels_) {
		  if (auto found = rules_->getByLabel()->find(r); found != rules_->getByLabel()->end()) {
			  auto trade = found->second->ruleSignal();
			  if (trade.has_value()) {
				  trades.push_back(std::move(trade.value()));
			  }
		  }
	  }
	  return trades.empty() ? std::nullopt : std::optional<std::vector<Trade>>{trades};
  }

  void Strategy::getMarketData(DataProcessorPtr data_processor_ptr){
	  //todo: make it multithreading
	  //todo: it may be different types of Data and Duration for the some strategies

	  for (const auto& i : indicator_labels_) {
		  if (auto p = indicators_->getByLabel()->find(i); p != indicators_->getByLabel()->end()) {
			  const auto& trading_contract = p->second->getTradingContract();
			  auto market_data = data_processor_ptr->getNewMarketData<
					  types::Value,
					  time_::Seconds>(trading_contract);
			  if (market_data.has_value()) {
				  std::cout << "new data for ticker: " << market_data->first << "; " << market_data->second << '\n';
				  indicators_->updateIndicator<types::Value, time_::Seconds>(p->second, market_data.value());
			  }
		  }
	  }
	  std::cout << "==========\n";
  }


  void Strategies::addStrategy (Strategy strategy) {
	  //todo: add check that indicator labels exist
	  const auto label = strategy.getLabel();

	  if (auto found = this->getByLabel()->find(label); found != this->getByLabel()->end()){
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy already exists; "));
	  }
	  auto new_strategy = Ptr<Strategy>(std::move(strategy));
	  this->getByLabel()->insert({new_strategy->getLabel(), std::move(new_strategy)});
  }

  void Strategies::removeStrategy (const types::String &label) {
	  auto found = this->getByLabel()->find(label);
	  if (found == this->getByLabel()->end()){
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy is not found; "));
	  }
	  else {
		  this->getByLabel()->erase(found);
	  }
  }

}//!namespace
