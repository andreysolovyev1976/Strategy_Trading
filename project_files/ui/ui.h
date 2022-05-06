//
// Created by Andrey Solovyev on 15/02/2022.
//

#pragma once

#include "tgbot/tgbot.h"
#include "bot_config.h"
#include "strategy_engine.h"
#include "ui_events.h"
#include "ui_init_data.h"
#include "ui_dashboard.h"

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
  using UserActivity = std::map<Username, Event>;
  using CurrentMessageId = int64_t;
  using UserCurrentMessage = std::map<Username, CurrentMessageId>;
  using UserChatId = std::map<Username, CurrentMessageId>;

  template <typename C>
  class UI final {
  public:
	  explicit UI(C* controller);
	  void run();
  private:
	  std::shared_ptr<C> c_ptr;
	  std::shared_ptr<TgBot::Bot> bot;
	  bool is_ui_ok = false;
	  algo::config::RobotConfig& robot_config;
	  algo::Engine& engine;
	  ObjectsCtorsData& ctors;

	  UserCurrentMessage current_message_id;
	  UserActivity user_activity;
	  UserChatId user_chat_id;

	  TgBot::InlineKeyboardButton::Ptr makeInlineCheckButton (const types::String& name) const;

	  struct Map { };
	  struct Vec { };
	  template <typename ConstIter, typename Type>
	  TgBot::InlineKeyboardMarkup::Ptr makeInlineKeyboard (Type type, ConstIter b, ConstIter e) const;
	  void hideInlineKeyboard (TgBot::CallbackQuery::Ptr query) const;
	  void updateKeyboard (TgBot::CallbackQuery::Ptr query) const;

	  void appendMessage (TgBot::Message::Ptr message, types::String msg) const;
	  void changeMessage (TgBot::Message::Ptr message, types::String msg) const;
	  void deleteMessage (TgBot::Message::Ptr message) const;

	  bool isQueryEventHandler (TgBot::CallbackQuery::Ptr query, Event event) const;
	  bool isMessageEventHandler (TgBot::Message::Ptr message, Event event) const;
	  bool isMessageResponseFor (TgBot::Message::Ptr message, const types::String& msg) const;
	  bool isMessageWithEventEmitted (TgBot::Message::Ptr message, Event event) const;

	  template <typename Keyboard>
	  void sendRequestForInput (TgBot::Chat::Ptr chat, Keyboard keyboard, const types::String& msg, Event event = Event::begin);
	  void sendMessageWithEvent (TgBot::Chat::Ptr chat, types::String msg, Event event);

	  bool isChatOk (TgBot::Message::Ptr message) const;

	  static const std::unordered_map<
			  Event,
			  void (UI::*)()
	  > INIT;
	  void initCommand (Event event);
	  void initHelp();
	  void initMainMenu();
	  void initTezosPrivateKeyMenu();
	  void initReplaceTezosPrivateKey();
	  void initIndicatorMenu();
	  void initAddIndicator();
	  void initRemoveIndicator();
	  void initSignalMenu();
	  void initAddSignal();
	  void initRemoveSignal();
	  void initStrategyMenu();
	  void initAddStrategy();
	  void initRemoveStrategy();
	  void initStopStrategy();
	  void initStartStrategy();
	  void initRuleMenu();
	  void initAddRule();
	  void initRemoveRule();
	  void initContractMenu();
	  void initAddContract();
	  void initRemoveContract();
	  void initStartOperations();
	  void initStopOperations();
	  void initStartUI();
	  void initStopUI();

	  /*
	  static const std::unordered_map<
			  types::String,
			  void (UI::*)()
	  > INDICATOR_MENU;
	  template <typename Obj>
	  void Add ();
	  template <typename Obj>
	  void List ();
	  template <typename Obj>
	  void Remove ();
	  static const std::unordered_map<
			  types::String,
			  void (UI::*)()
	  > SIGNAL_MENU;
	  static const std::unordered_map<
			  types::String,
			  void (UI::*)()
	  > RULE_MENU;
	  static const std::unordered_map<
			  types::String,
			  void (UI::*)()
	  > STRATEGY_MENU;
*/
  };

}//!namespace

#include "ui.tcc"

#endif //STRATEGY_TRADING_TG_BOT_UI_H
