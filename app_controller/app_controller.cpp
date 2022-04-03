//
// Created by Andrey Solovyev on 03/04/2022.
//

#include "app_controller.h"


using namespace user_interface;

void Controller::init() {
	ui.init(&engine, this);
}
void Controller::run() {
	ui.run();
}


const std::unordered_map<
		Event,
		String (Controller::*)(const types::String&)
> Controller::EVENT_HANDLER = {
		{Event::help, &Controller::help},
		{Event::addIndicator, &Controller::addIndicator},
		{Event::removeIndicator, &Controller::removeIndicator},
		{Event::getIndicators, &Controller::getIndicators},
		{Event::addSignal, &Controller::addSignal},
		{Event::removeSignal, &Controller::removeSignal},
		{Event::getSignals, &Controller::getSignals},
		{Event::addStrategy, &Controller::addStrategy},
		{Event::addRule, &Controller::addRule},
		{Event::removeRule, &Controller::removeRule},
		{Event::getRules, &Controller::getRules},
		{Event::removeStrategy, &Controller::removeStrategy},
		{Event::stopStrategy, &Controller::stopStrategy},
		{Event::startStrategy, &Controller::startStrategy},
		{Event::startOperations, &Controller::startOperations},
		{Event::stopOperations, &Controller::stopOperations},
		{Event::startUI, &Controller::startUI},
		{Event::stopUI, &Controller::stopUI},
		{Event::getContracts, &Controller::getContracts},
};

String Controller::processEvent (Event event, const types::String& input) {
	if (auto event_exists = EVENT_HANDLER.find(event); event_exists != EVENT_HANDLER.end()) {
		return (this->*EVENT_HANDLER.at(event))(input);
	}
	else return String{};
}
String Controller::help([[maybe_unused]] const types::String& input){
	types::String result = R"(
1) Create an Indicator, that holds and updates Market Data for a ticker, either Quipuswap or Coinbase.
2) Create a Signal, that compares two Indicators or an Indicator and a threshold.
3) Create a Rule, that determines what to do in the market once Signal triggers action.
4) Create a Strategy, that may hold several Rules, helping to organize complex behaviour for the Robot.
5) Each strategy should be started separately. Each strategy can be either stopped or started.
)";
	return result;
}
String Controller::addIndicator([[maybe_unused]] const types::String& input){
	if (not init_indicator.isInitialized()) return "Indicator is not initialized"s;

//	  using _K = Timestamp<Minutes>;
//	  using _V = Quote<types::Value, Minutes>;
//	  using _MarketDataContainer = SingleThreadedLimitedSizeMap<_K, _V>;
	Indicator indicator (
			init_indicator.label,
			init_indicator.ticker,
			init_indicator.trade_side);
	engine.addTradingObject(std::move(indicator));
	init_indicator.clear();
	return "Indicator successfully added"s;
}
String Controller::removeIndicator([[maybe_unused]] const types::String& input){
	return "WIP - Removing Indicator";
}
String Controller::getIndicators([[maybe_unused]] const types::String& input){
	auto* indicators = engine.getPtr<Indicators>();
	if (indicators->getByLabel()->Empty()) return "No indicators yet"s;
	String result;
	result.reserve(100); //todo: update for a const value
	for (const auto& [label, owner] : *(indicators->getByLabel()) ) {
		result += "Label: ";
		result += label;
		result += "; Ticker: ";
		result += owner->getTicker();
		result += "\n";
	}
	return result;
}
String Controller::addSignal([[maybe_unused]] const types::String& input){
	if (not init_signal.isInitialized()) return "Signal is not initialized"s;

	Signal signal (
			init_signal.label,
			init_signal.signal_type,
			init_signal.relation,
			init_signal.indicator_labels,
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(signal));
	init_signal.clear();
	return "Signal successfully added"s;
}
String Controller::removeSignal([[maybe_unused]] const types::String& input){
	return "WIP - Removing Signal";
}
String Controller::getSignals([[maybe_unused]] const types::String& input){
	auto* signals = engine.getPtr<Signals>();
	if (signals->getByLabel()->Empty()) return "No signals yet"s;
	String result;
	result.reserve(100); //todo: update for a const value
	for (const auto& [label, owner] : *(signals->getByLabel()) ) {
		result += "Label: ";
		result += label;
		result += "; Signal type: ";
		result += signal_base::SignalTypeToString(owner->getSignalType());
		result += "\n";
	}
	return result;
}
String Controller::addStrategy([[maybe_unused]] const types::String& input){
	if (not init_strategy.isInitialized()) return "Strategy is not initialized"s;

	Strategy strategy (init_strategy.label,
			engine.getPtr<Indicators>(),
			engine.getPtr<Signals>(),
			engine.getPtr<Rules>()
	);
	auto* rules = engine.getPtr<Rules>();
	for (const auto& rule_label : init_strategy.rules_labels) {
		if (auto found = rules->getPtr(rule_label); found) {
			strategy.addRule(rule_label);
		}
	}
	if (not strategy.isInitialized()) {
		return "None of the Rules' Labels is found, please check"s;
	}
	engine.addTradingObject<Strategy>(std::move(strategy));
	init_strategy.clear();
	return "Strategy successfully added"s;
}
String Controller::removeStrategy([[maybe_unused]] const types::String& input){
	return "WIP - Removing Strategy"s;
}
String Controller::addRule([[maybe_unused]] const types::String& input){
	if (not init_rule.isInitialized()) return "Rule is not initialized"s;

	Rule rule (
			init_rule.label,
			init_rule.ticker,
			init_rule.quipuswap_trade_side,
//			  init_rule.rule_type,
			init_rule.signal_label,
			init_rule.signal_value,
//			  init_rule.position_side,
			init_rule.order_quantity,
//			  init_rule.trade_type,
//			  init_rule.order_type,
			engine.getPtr<Signals>()
	);

	engine.addTradingObject<Rule>(std::move(rule));
	init_strategy.clear();
	return "Rule successfully added"s;
}
String Controller::removeRule([[maybe_unused]] const types::String& input){
	return "WIP - Removing Rule";
}
String Controller::getRules([[maybe_unused]] const types::String& input){
	auto* rules = engine.getPtr<Rules>();
	if (rules->getByLabel()->Empty()) return "No rules yet"s;
	String result;
	result.reserve(100); //todo: update for a const value
	for (const auto& [label, owner] : *(rules->getByLabel()) ) {
		result += "Label: ";
		result += label;
		result += "; Trading ticker: ";
		result += owner->getTicker();
		result += "\n";
	}
	return result;
}
String Controller::stopStrategy([[maybe_unused]] const types::String& input){
	engine.deactivateStrategy(input);

	String result = "Strategy with Label ";
	result += input;
	result += " is offline";
	return result;
}
String Controller::startStrategy([[maybe_unused]] const types::String& input){
	engine.activateStrategy(input);

	String result = "Strategy with Label ";
	result += input;
	result += " is online";
	return result;
}
String Controller::startOperations([[maybe_unused]] const types::String& input){
	return "WIP - Starting the Operations";
}
String Controller::stopOperations([[maybe_unused]] const types::String& input){
	return "WIP - Stopping the Operations";
}
String Controller::startUI([[maybe_unused]] const types::String& input){
	return "WIP - Start the UI";
}
String Controller::stopUI([[maybe_unused]] const types::String& input){
	return "WIP - Stop the UI";
}
String Controller::addContract([[maybe_unused]] const types::String& input){
	std::stringstream ss;
	robot_config.printIniFile(ss);
	return ss.str();
/*
	  config::ContractInfo ci;
	  ci.name = "name5";
	  ci.ticker_cb = "CB 5";
	  ci.ticker_qs = "QS 5";
	  ci.price_diff = 42;
	  ci.timeframe = 42;
	  ci.trade_size = 42;
	  ci.slippage = 4;

	  config.updateIni(std::move(ci));
	  config.loadFromIni();
	  config.printIniFile(std::cerr);
	  config.printSettings(std::cerr);

	  auto c5 = config.getContractInfo("name5");
	  std::cerr << c5 << '\n';

	  auto fuck = config.getContractInfo("fuck");
	  std::cerr << fuck << '\n';
	  */
}
String Controller::getContracts([[maybe_unused]] const types::String& input){
	std::stringstream ss;
	robot_config.printIniFile(ss);
	return ss.str();
/*
	  config::ContractInfo ci;
	  ci.name = "name5";
	  ci.ticker_cb = "CB 5";
	  ci.ticker_qs = "QS 5";
	  ci.price_diff = 42;
	  ci.timeframe = 42;
	  ci.trade_size = 42;
	  ci.slippage = 4;

	  config.updateIni(std::move(ci));
	  config.loadFromIni();
	  config.printIniFile(std::cerr);
	  config.printSettings(std::cerr);

	  auto c5 = config.getContractInfo("name5");
	  std::cerr << c5 << '\n';

	  auto fuck = config.getContractInfo("fuck");
	  std::cerr << fuck << '\n';
	  */
}
