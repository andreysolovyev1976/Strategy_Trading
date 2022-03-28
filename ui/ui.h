//
// Created by Andrey Solovyev on 15/02/2022.
//

#pragma once

#include "tgbot/tgbot.h"

#include "types_declarations.h"
#include "user_defined_exceptions.h"
#include "bot_config.h"

#include "ui_init_data.h"
#include "ui_events.h"

#include "strategy_engine.h"

#include <map>
#include <vector>
#include <thread>
#include <csignal>
#include <cstdio>
#include <cstdlib>

#ifndef STRATEGY_TRADING_TG_BOT_UI_H
#define STRATEGY_TRADING_TG_BOT_UI_H

namespace user_interface {
  using namespace std::literals;

  using Username = types::String;
  using UserUI_Activity = std::map<Username, Event>;

  class Controller final {
	  using Commands = std::map<types::String, TgBot::BotCommand::Ptr>;
	  using Keyboards = std::map<types::String, TgBot::GenericReply::Ptr>;
  public:
	  Controller();
	  void run();

	  void addCommand (types::String name, types::String description = ""s);
	  void getCommand (types::String name) const;
	  void removeCommand (types::String name);
	  void executeCommand (types::String name);
	  void getCommands ();

  private:
	  bool is_initialized = false;
	  const types::String token_;
	  TgBot::Bot bot;
	  Commands commands;
	  [[maybe_unused]] Keyboards keyboards;
	  TgBot::InlineKeyboardButton::Ptr makeInlineCheckButton (const types::String& name) const;

	  static const std::unordered_map<
			  Event,
			  void (Controller::*)()
	  > INIT;
	  void initCommand (Event event);
	  algo::config::RobotConfig& robot_config;
	  void initRobot();
	  void initAddIndicator();
	  void initRemoveIndicator();
	  void initGetIndicators();
	  void initAddSignal();
	  void initRemoveSignal();
	  void initGetSignals();
	  void initAddStrategy();
	  void initRemoveStrategy();
	  void initAddRule();
	  void initRemoveRule();
	  void initGetRules();
	  void initStopStrategy();
	  void initStartStrategy();
	  void initStartOperations();
	  void initStopOperations();
	  void initStartUI();
	  void initStopUI();
	  void initGetContracts();

	  static const std::unordered_map<
			  Event,
			  types::String (Controller::*)(const types::String&)
	  > EVENT_HANDLER;
	  types::String processEvent (Event event, const types::String& input = ""s);
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
	  types::String getContracts(const types::String& input);

	  InitIndicator init_indicator;
	  InitSignal init_signal;
	  InitRule init_rule;
	  InitStrategy init_strategy;

	  algo::Engine engine;

	  UserUI_Activity user_ui_activity;

	  void runBot();
	  std::map<std::thread::id, std::thread> threads;
	  std::map<types::String, std::thread::id> strategies_to_threads;
	  std::set<types::String> active_strategies;
  };



}//!namespace
#endif //STRATEGY_TRADING_TG_BOT_UI_H
