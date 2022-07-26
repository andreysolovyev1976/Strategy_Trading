//
// Created by Andrey Solovyev on 03/04/2022.
//

#include "controller.h"


using namespace types;
using namespace algo;
using namespace user_interface;


Controller::Controller ()
		: robot_config(config::RobotConfig::getInstance(const_values::CONFIG_FILENAME))
		, bot (std::make_unique<TgBot::Bot>(const_values::TG_BOT_TOKEN))
		, engine (bot.get())
{}

void Controller::init(){
	robot_config.loadFromIni();
	ui = std::make_unique<user_interface::UI<Controller>>(this);
	is_initialized = true;
}

void Controller::run() {
	if (is_initialized)
		ui->run(); //todo make it a separate thread
}

TgBot::Bot* Controller::getBotPtr() {
	return bot.get();
}

algo::config::RobotConfig& Controller::getConfig() {
	return robot_config;
}

algo::Engine& Controller::getEngine(){
	return engine;
}

user_interface::ObjectsCtorsData& Controller::getCtors() {
	return ctors;
}

const std::unordered_map<
		Event,
		String (Controller::*)(const types::String&)
> Controller::EVENT_HANDLER = {
		{Event::help, &Controller::help},
		{Event::replaceTezosPrivateKey, &Controller::replaceTezosPrivateKey},
		{Event::getTezosPrivateKey, &Controller::getTezosPrivateKey},
		{Event::addIndicator, &Controller::addIndicator},
		{Event::removeIndicator, &Controller::removeIndicator},
		{Event::getIndicators, &Controller::getIndicators},
		{Event::addSignal, &Controller::addSignal},
		{Event::removeSignal, &Controller::removeSignal},
		{Event::getSignals, &Controller::getSignals},
		{Event::addStrategy, &Controller::addStrategy},
		{Event::removeStrategy, &Controller::removeStrategy},
		{Event::getStrategies, &Controller::getStrategies},
		{Event::stopStrategy, &Controller::stopStrategy},
		{Event::startStrategy, &Controller::startStrategy},
		{Event::addRule, &Controller::addRule},
		{Event::removeRule, &Controller::removeRule},
		{Event::getRules, &Controller::getRules},
		{Event::addContract, &Controller::addContract},
		{Event::removeContract, &Controller::removeContract},
		{Event::getContracts, &Controller::getContracts},
		{Event::startOperations, &Controller::startOperations},
		{Event::stopOperations, &Controller::stopOperations},
		{Event::startUI, &Controller::startUI},
		{Event::stopUI, &Controller::stopUI},
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
String Controller::replaceTezosPrivateKey(const types::String& input){
	if (not input.empty()) {
		robot_config.updateIni(input);
		return "Updated successfully"s;
	}
	else {
		throw InvalidArgumentError(EXCEPTION_MSG("Can't proceed an empty key; "));
	}
}
String Controller::getTezosPrivateKey([[maybe_unused]]const types::String& input){
	return robot_config.getKey();
}
String Controller::addIndicator([[maybe_unused]] const types::String& input){
	if (not ctors.init_indicator.isInitialized()) return "Indicator is not initialized"s;

//	  using _K = Timestamp<Minutes>;
//	  using _V = Quote<types::Value, Minutes>;
//	  using _MarketDataContainer = SingleThreadedLimitedSizeMap<_K, _V>;

	if (ctors.init_indicator.modifier == "none") {
		Indicator indicator (
				ctors.init_indicator.label,
				ctors.init_indicator.ticker,
				ctors.init_indicator.trade_side);
		engine.addTradingObject(std::move(indicator));
		ctors.init_indicator.clear();
	}
	else {
		//todo: type deduction must aligned with Indicator type - amend it later
		Modifier<types::Value> modifier (ctors.init_indicator.label);
		modifier.setModifierValue(types::Value(ctors.init_indicator.modifier_value));

		Indicator indicator (
				ctors.init_indicator.label,
				ctors.init_indicator.ticker,
				ctors.init_indicator.trade_side,
				modifier.getModifierMethod(ctors.init_indicator.modifier)
		);

		engine.addTradingObject(std::move(modifier));
		engine.addTradingObject(std::move(indicator));
		ctors.init_indicator.clear();
	}
	return "Indicator successfully added"s;
}
String Controller::removeIndicator([[maybe_unused]] const types::String& input){
	auto* indicators = engine.getPtr<Indicators>();
	auto* modifiers = engine.getPtr<Modifiers<types::Value>>();

	if (auto found = indicators->getByLabel()->find(input); found==indicators->getByLabel()->end()) {
		return "No such Indicator"s;
	}else {
		indicators->getByLabel()->erase(found);
		if (auto found_mod = modifiers->getByLabel()->find(input); found_mod==modifiers->getByLabel()->end()) {
			modifiers->getByLabel()->erase(found_mod);
		}

	}
	return "Indicator removed"s;
}
String Controller::getIndicators([[maybe_unused]] const types::String& input){
	auto* indicators = engine.getPtr<Indicators>();
	String result;
	result.reserve(100); //todo: update for a const value

	for (auto curr = indicators->getByLabel()->begin(), e = indicators->getByLabel()->end(); curr != e; ++curr) {
		const auto& [label, owner] = *curr;
		result += "Label: ";
		result += label;
		result += "; Ticker: ";
		result += owner->getTicker();
		result += "\n";
	}
	return result;
}
String Controller::addSignal([[maybe_unused]] const types::String& input){
	if (not ctors.init_signal.isInitialized()) return "Signal is not initialized"s;

	Signal signal (
			ctors.init_signal.label,
			ctors.init_signal.signal_type,
			ctors.init_signal.relation,
			ctors.init_signal.indicator_labels,
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(signal));
	ctors.init_signal.clear();
	return "Signal successfully added"s;
}
String Controller::removeSignal([[maybe_unused]] const types::String& input) {
	auto* signals = engine.getPtr<Signals>();
	if (auto found = signals->getByLabel()->find(input); found==signals->getByLabel()->end()) {
		return "No such Signal"s;
	} else {
		signals->getByLabel()->erase(found);
	}
	return "Signal removed"s;
}
String Controller::getSignals([[maybe_unused]] const types::String& input){
	auto* signals = engine.getPtr<Signals>();
	if (signals->getByLabel()->empty()) return "No signals yet"s;
	String result;
	result.reserve(100); //todo: update for a const value

	for (auto curr = signals->getByLabel()->begin(), e = signals->getByLabel()->end(); curr != e; ++curr) {
		const auto& [label, owner] = *curr;
		result += "Label: ";
		result += label;
		result += "; Signal type: ";
		result += signal_base::SignalTypeToString(owner->getSignalType());
		result += "\n";
	}
	return result;
}
String Controller::addStrategy([[maybe_unused]] const types::String& input){
	if (not ctors.init_strategy.isInitialized()) return "Strategy is not initialized"s;

	Strategy strategy (ctors.init_strategy.label,
			engine.getPtr<Indicators>(),
			engine.getPtr<Signals>(),
			engine.getPtr<Rules>(),
			types::fromChars(input)
	);
	auto* rules = engine.getPtr<Rules>();
	for (const auto& rule_label : ctors.init_strategy.rules_labels) {
		if (rules->objectExists(rule_label)) {
			strategy.addRule(rule_label);
		}
	}
	if (not strategy.isInitialized()) {
		return "None of the Rules' Labels is found, please check"s;
	}
	engine.addTradingObject<Strategy>(std::move(strategy));
	ctors.init_strategy.clear();
	return "Strategy successfully added"s;
}
String Controller::removeStrategy([[maybe_unused]] const types::String& input){
	ActiveStrategy active_strategy (input, robot_config.getKey());
	auto* strategies = engine.getPtr<Strategies>();
	types::String output;
	if (auto found = strategies->getByLabel()->find(input); found == strategies->getByLabel()->end()) {
		output = "No such Strategy";
	} else {
		if (engine.isStrategyActive(active_strategy)) {
			engine.deactivateStrategy(active_strategy);
			output += "Strategy stopped. ";
		}
		strategies->getByLabel()->erase(found);
		output += "Strategy removed";
	}
	return output;
}
String Controller::getStrategies([[maybe_unused]] const types::String& input){
	auto* strategies = engine.getPtr<Strategies>();
	if (strategies->getByLabel()->empty()) return "No strategies yet"s;

	types::String result;
	result.reserve(100); //todo: update for a const value

	for (auto curr = strategies->getByLabel()->begin(), e = strategies->getByLabel()->end(); curr != e; ++curr) {
		const auto& [label, owner] = *curr;
		result += "Label: ";
		result += label;
		result += "\n";
	}
	return result;
}
String Controller::stopStrategy([[maybe_unused]] const types::String& input){
	ActiveStrategy active_strategy (input, robot_config.getKey());
	engine.deactivateStrategy(active_strategy);

	String result = "Strategy with Label ";
	result += input;
	result += " is offline";
	return result;
}
String Controller::startStrategy([[maybe_unused]] const types::String& input){
	ActiveStrategy active_strategy (input, robot_config.getKey());
	engine.activateStrategy(std::move(active_strategy));

	String result = "Strategy with Label ";
	result += input;
	result += " is online";
	return result;
}
String Controller::addRule([[maybe_unused]] const types::String& input){
	if (not ctors.init_rule.isInitialized()) return "Rule is not initialized"s;

	Rule rule (
			ctors.init_rule.label,
			ctors.init_rule.ticker,
			ctors.init_rule.quipuswap_trade_side,
//			  ctors.init_rule.rule_type,
			ctors.init_rule.signal_label,
			ctors.init_rule.signal_value,
//			  ctors.init_rule.position_side,
			ctors.init_rule.order_quantity,
			ctors.init_rule.slippage,
//			  ctors.init_rule.trade_type,
//			  ctors.init_rule.order_type,
			engine.getPtr<Signals>()
	);

	engine.addTradingObject<Rule>(std::move(rule));
	ctors.init_strategy.clear();
	return "Rule successfully added"s;
}
String Controller::removeRule([[maybe_unused]] const types::String& input) {
	auto* rules = engine.getPtr<Rules>();
	types::String output;
	if (auto found = rules->getByLabel()->find(input); found==rules->getByLabel()->end()) {
		output = "No such Rule";
	} else {
		rules->getByLabel()->erase(found);
		output = "Rule removed";
	}
	return output;
}
String Controller::getRules([[maybe_unused]] const types::String& input){
	auto* rules = engine.getPtr<Rules>();
	if (rules->getByLabel()->empty()) return "No rules yet"s;
	String result;
	result.reserve(100); //todo: update for a const value
	for (auto curr = rules->getByLabel()->begin(), e = rules->getByLabel()->end(); curr != e; ++curr) {
		const auto& [label, owner] = *curr;
		result += "Label: ";
		result += label;
		result += "; Trading ticker: ";
		result += owner->getTicker();
		result += "\n";
	}
	return result;
}
String Controller::addContract([[maybe_unused]] const types::String& input){
	if (not ctors.init_contract.isInitialized()) return "Contract is not initialized"s;

	config::ContractInfo ci;
	ci.name = ctors.init_contract.label;
	ci.ticker_cb = ctors.init_contract.ticker_cb;
	ci.ticker_qs = ctors.init_contract.ticker_qs;

	robot_config.updateIni(std::move(ci));
	ctors.init_contract.clear();
	return "Contract list was updated"s;
}
String Controller::removeContract([[maybe_unused]] const types::String& input) {
	return "";
}
String Controller::getContracts([[maybe_unused]] const types::String& input){
	std::stringstream ss;
	robot_config.loadFromIni();
	robot_config.printContracts(ss);
	return ss.str();
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
