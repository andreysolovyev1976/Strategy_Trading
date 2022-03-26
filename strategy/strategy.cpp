//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "strategy.h"
#include "user_defined_exceptions.h"



namespace algo {

  Strategy::Strategy (types::String label, Account* account)
		  : label_ (std::move(label))
		  , account_(account)
  {}

  void Strategy::addIndicator(const Ticker & ticker, types::String indicator_label, MarketDataContainer input_value, ModifierFunc modifier) {
	  indicators_tickers_.emplace(ticker);
	  Indicator indicator (ticker, std::move(indicator_label), std::move(input_value), modifier);
	  indicators_.addIndicator(std::move(indicator));
  }
  void Strategy::addIndicator(const Ticker & ticker, types::String indicator_label, MarketDataContainer input_value) {
	  indicators_tickers_.emplace(ticker);
	  Indicator indicator (ticker, std::move(indicator_label), std::move(input_value));
	  indicators_.addIndicator(std::move(indicator));
  }
  void Strategy::addIndicator(const Ticker & ticker, types::String indicator_label, ModifierFunc modifier) {
	  indicators_tickers_.emplace(ticker);
	  Indicator indicator (ticker, std::move(indicator_label), modifier);
	  indicators_.addIndicator(std::move(indicator));
  }
  void Strategy::addIndicator(const Ticker & ticker, types::String indicator_label) {
	  indicators_tickers_.emplace(ticker);
	  Indicator indicator (ticker, std::move(indicator_label));
	  indicators_.addIndicator(std::move(indicator));
  }
  void Strategy::addSignal(
		  types::String signal_label,
		  types::String signal_type,
		  types::String relation,
		  std::vector<types::String> indicator_labels
  ) {
	  Signal signal (
			  std::move(signal_label),
			  std::move(signal_type),
			  std::move(relation),
			  std::move(indicator_labels),
			  &indicators_);
	  signals_.addSignal(std::move(signal));
  }

  void Strategy::addRule (
		  Ticker ticker,
		  types::String rule_label,
		  types::String rule_type,
		  const types::String& signal_label,
		  int signal_value,
		  types::String position_side,
		  trade_base::OrderQuantity order_quantity,
		  types::String trade_type,
		  types::String order_type )
  {
	  Rule rule (
			  ticker,
			  std::move(rule_label),
			  std::move(rule_type),
			  signal_label, //todo: do I need this in ctor? Comes with ptr_signal
			  signal_value, //todo: do I need this in ctor? Comes with ptr_signal
			  signals_.shareObject(signal_label),
			  std::move(position_side),
			  std::move(order_quantity),
			  std::move(trade_type),
			  std::move(order_type),
			  &signals_);

	  rules_.addRule(std::move(rule));
  }

  const Indicators& Strategy::getIndicators() const {return indicators_;}
  const Signals& Strategy::getSignals() const {return signals_;}
  const Rules& Strategy::getRules() const {return rules_;}
  bool Strategy::isInitialized() const {return not rules_.getByLabel()->Empty();}
  const types::String& Strategy::getLabel () const {return label_;}

  void Strategy::getMarketData(){
	  //todo: make it multithreading
	  //todo: it may be different types of Data and Duration for the some strategies
	  for (const auto &ticker : indicators_tickers_){
		  auto market_data = DataProcessor_::getNewMarketData<types::Value, time_::Seconds>(ticker);
		  std::cout << "new data for ticker: " << market_data.first << "; " << market_data.second << '\n';
		  indicators_.updateIndicators<types::Value, time_::Seconds>(market_data);
	  }
	  std::cout << "==========\n";
  }


  std::optional<Trade> Strategy::ruleSignal () {
	  std::vector<Trade> trades;

	  for (auto& rule : *rules_.getByLabel()) {
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
		  throw IvalidArgumentError(EXCEPTION_MSG("Strategy already exists; "));
	  }
	  auto new_strategy = std::make_unique<Strategy>(std::move(strategy));
	  this->getByLabel()->Insert({new_strategy->getLabel(), std::move(new_strategy)});
  }

  void Strategies::removeStrategy (const types::String &label) {
	  auto found = this->getByLabel()->Find(label);
	  if (found == this->getByLabel()->end()){
		  throw IvalidArgumentError(EXCEPTION_MSG("Strategy is not found; "));
	  }
	  else {
		  this->getByLabel()->Erase(found);
	  }
  }

}//!namespace
