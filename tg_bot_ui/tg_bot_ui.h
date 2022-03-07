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

  class TgBotUI final {
	  using Commands = std::map<const types::String&, TgBot::BotCommand::Ptr>;
  public:
	  TgBotUI(types::String token = "5217629109:AAGO8OQLcGbdNhnhBSuPr6ywKsE-d2Dib4U");
	  void addCommand (types::String name, types::String description = "");
	  void getCommand (types::String name) const;
	  void removeCommand (types::String name);
	  void executeCommand (types::String name);

	  void postData (types::String text);

  private:
	  const types::String token_;
	  types::UInt chat_id_, bot_id_;
	  TgBot::Bot bot;

	  types::String makeCommand (const TgBot::BotCommand::Ptr bot_command);
  };

}//!namespace
#endif //STRATEGY_TRADING_TG_BOT_UI_H
