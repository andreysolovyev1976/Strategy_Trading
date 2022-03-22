//
// Created by Andrey Solovyev on 15/02/2022.
//

#pragma once

#include "types_declarations.h"
#include "user_defined_exceptions.h"

#include <csignal>
#include <cstdio>
#include <cstdlib>

#include "tgbot/tgbot.h"

#ifndef STRATEGY_TRADING_TG_BOT_UI_H
#define STRATEGY_TRADING_TG_BOT_UI_H

namespace tg_bot {

  namespace {
  }//noname namespace

  class TgBotUI final {
	  using Commands = std::map<types::String, TgBot::BotCommand::Ptr>;
  public:
	  TgBotUI();
	  void init ();
	  void run();

	  void addCommand (types::String name, types::String description = "");
	  void getCommand (types::String name) const;
	  void removeCommand (types::String name);
	  void executeCommand (types::String name);

	  void postData (types::String text);

	  void getCommands ();

  private:
	  const types::String token_;
	  types::UInt chat_id_;

	  bool is_initialized = false;

	  TgBot::Bot bot;
	  Commands commands;


/*
	  static const std::unordered_map<
			  ValueType,
			  ValueObject (ValueGenerator::*)(const ValueObject&, ValueType)
	  > RHS_SIMPLE_OPERATION_DIV_GENERATOR;
*/
  };

}//!namespace
#endif //STRATEGY_TRADING_TG_BOT_UI_H
