//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "strategy.h"
#include "user_defined_exceptions.h"

namespace algo {

  Strategy::Strategy (types::String label,
		  Indicators* indicators,
		  Signals* signals,
		  Rules* rules,
		  Account* account)
		  : label_ (std::move(label))
		  , indicators_ (indicators)
		  , signals_(signals)
		  , rules_(rules)
		  , account_(account)
  {}

  void Strategy::addRule (const types::String& label) {
	  if (rule_labels_.count(label)) throw InvalidArgumentError(EXCEPTION_MSG("Rule Label already exists in this Strategy; "));
	  if (auto found = rules_->getByLabel()->Find(label); found != rules_->getByLabel()->End()) {
		  rule_labels_.insert(label);
		  for (const auto& item : found->second->getIndicatorsLabels()) {
			  indicators_tickers_.insert(indicators_->getObject(item).getTicker());
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

  void Strategy::getMarketData(){
	  //todo: make it multithreading
	  //todo: it may be different types of Data and Duration for the some strategies
	  for (const auto &ticker : indicators_tickers_){
		  auto market_data = DataProcessor_::getNewMarketData<types::Value, time_::Seconds>(ticker);
		  std::cout << "new data for ticker: " << market_data.first << "; " << market_data.second << '\n';
		  indicators_->updateIndicators<types::Value, time_::Seconds>(market_data);
	  }
	  std::cout << "==========\n";
  }


  std::optional<Trade> Strategy::ruleSignal () {
	  if (not isInitialized())
	  	throw RuntimeError(EXCEPTION_MSG("No Rules selected; "));

	  std::vector<Trade> trades;

	  for (auto& rule : *(rules_->getByLabel()) ) {
		  auto trade = rule.second->ruleSignal();
		  if (trade.has_value()) {
			  trades.push_back(std::move(trade.value()));
		  }
	  }

	  //todo: plugin implementation, priority of trades is required - select tickers!!!
	  if (not trades.empty()){
		  return *begin(trades);
	  }
	  else return std::nullopt;
  }


  void Strategies::addStrategy (Strategy strategy) {
	  //todo: add check that indicator labels exist
	  const auto label = strategy.getLabel();

	  if (auto found = this->getByLabel()->Find(label); found != this->getByLabel()->end()){
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy already exists; "));
	  }
	  auto new_strategy = std::make_unique<Strategy>(std::move(strategy));
	  this->getByLabel()->Insert({new_strategy->getLabel(), std::move(new_strategy)});
  }

  void Strategies::removeStrategy (const types::String &label) {
	  auto found = this->getByLabel()->Find(label);
	  if (found == this->getByLabel()->end()){
		  throw InvalidArgumentError(EXCEPTION_MSG("Strategy is not found; "));
	  }
	  else {
		  this->getByLabel()->Erase(found);
	  }
  }

}//!namespace
