//
// Created by Andrey Solovyev on 03/04/2022.
//

#pragma once

#include "ui_init_data.h"
#include "ui_events.h"
#include "ui.h"
#include "strategy_engine.h"
#include "bot_config.h"

#ifndef STRATEGY_TRADING_APP_CONTROLLER_H
#define STRATEGY_TRADING_APP_CONTROLLER_H

using namespace std::string_literals;

class Controller final {
public:
	Controller();
	void init ();
	void run();

	types::String processEvent (user_interface::Event event, const types::String& input = ""s);

	algo::config::RobotConfig& getConfig();
	TgBot::Bot* getBotPtr();
	algo::Engine& getEngine();
	user_interface::ObjectsCtorsData& getCtors();

private:
	algo::config::RobotConfig& robot_config;
	std::unique_ptr<TgBot::Bot> bot;
	algo::Engine engine;
	user_interface::ObjectsCtorsData ctors;
	std::unique_ptr<user_interface::UI<Controller>> ui;
	bool is_initialized = false;

	static const std::unordered_map<
			user_interface::Event,
			types::String (Controller::*)(const types::String&)
	> EVENT_HANDLER;

	types::String help(const types::String& input);
	types::String addIndicator(const types::String& input);
	types::String removeIndicator(const types::String& input);
	types::String getIndicators(const types::String& input);
	types::String addSignal(const types::String& input);
	types::String removeSignal(const types::String& input);
	types::String getSignals(const types::String& input);
	types::String addStrategy(const types::String& input);
	types::String removeStrategy(const types::String& input);
	types::String addRule(const types::String& input);
	types::String removeRule(const types::String& input);
	types::String getRules(const types::String& input);
	types::String stopStrategy(const types::String& input);
	types::String startStrategy(const types::String& input);
	types::String startOperations(const types::String& input);
	types::String stopOperations(const types::String& input);
	types::String startUI(const types::String& input);
	types::String stopUI(const types::String& input);
	types::String addContract(const types::String& input);
	types::String getContracts(const types::String& input);

};

#endif //STRATEGY_TRADING_APP_CONTROLLER_H
