//
// Created by Andrey Solovyev on 15/02/2022.
//

#include "ui.h"
#include "const_values.h"
#include "timestamp.h"

#include <vector>
#include <sstream>

namespace user_interface {

  using namespace TgBot;
  using namespace std::literals;
  using namespace types;
  using namespace algo;
  using namespace algo::time_;

  UI::UI()
		  : token_ (const_values::TG_BOT_TOKEN)
		  , bot (token_)
		  , robot_config(config::RobotConfig::getInstance(const_values::CONFIG_FILENAME))
		  , engine (&bot)
  {
	  for (Event curr = Event::begin, last = Event::end; curr != last; curr = nextEvent(curr)){
		  initCommand(curr);
	  }
	  is_ui_initialized = true;
  }//!ctor

  void UI::run() {
	  if (not is_ui_initialized) {
		  throw LogicError(EXCEPTION_MSG("TG Bot is not initialized "));
	  }
		  try {
			  bot.getApi().deleteWebhook();

			  TgLongPoll longPoll(bot);
			  while (true) {
				  longPoll.start();
			  }
		  }
		  catch (std::exception& e) {
			  if (chat_id.has_value())
				  bot.getApi().sendMessage(chat_id.value(), EXCEPTION_MSG("TG Bot caught exception: "s+e.what()+" "));
#ifdef CERR_OVER_BOT
			  else std::cerr << e.what() << '\n';
#endif
#ifdef BOT_OVER_CERR
		  throw RuntimeError(EXCEPTION_MSG("TG Bot caught exception: "s + e.what() + " "));
#endif
		  }
  }//!func

  const std::unordered_map<
		  Event,
		  void (UI::*)()
  > UI::INIT = {
		  {Event::help, &UI::initHelp},
		  {Event::addIndicator, &UI::initAddIndicator},
		  {Event::removeIndicator, &UI::initRemoveIndicator},
		  {Event::getIndicators, &UI::initGetIndicators},
		  {Event::addSignal, &UI::initAddSignal},
		  {Event::removeSignal, &UI::initRemoveSignal},
		  {Event::getSignals, &UI::initGetSignals},
		  {Event::addStrategy, &UI::initAddStrategy},
		  {Event::removeStrategy, &UI::initRemoveStrategy},
		  {Event::addRule, &UI::initAddRule},
		  {Event::removeRule, &UI::initRemoveRule},
		  {Event::getRules, &UI::initGetRules},
		  {Event::stopStrategy, &UI::initStopStrategy},
		  {Event::startStrategy, &UI::initStartStrategy},
		  {Event::startOperations, &UI::initStartOperations},
		  {Event::stopOperations, &UI::initStopOperations},
		  {Event::startUI, &UI::initStartUI},
		  {Event::stopUI, &UI::initStopUI},
		  {Event::addContract, &UI::initAddContract},
		  {Event::getContracts, &UI::initGetContracts},
		  {Event::initRobot, &UI::initRobot},
  };
  void UI::initCommand (Event event) {
	  if (auto event_exists = INIT.find(event); event_exists != INIT.end()) {
		  (this->*INIT.at(event))();
	  }
  }
  void UI::initRobot() {
	  robot_config.loadFromIni();
  }
  void UI::initHelp(){
	  bot.getEvents().onCommand("help", [this](Message::Ptr message) {
		chat_id = message->chat->id;
		bot.getApi().sendMessage(message->chat->id, processEvent(Event::help));
	  });
  }
  void UI::initAddIndicator(){
	  bot.getEvents().onCommand("add_indicator", [this](Message::Ptr message) {
		chat_id = message->chat->id;
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot.getApi().sendMessage(message->chat->id,
				"Label new Indicator", false, 0, keyboard_reply);
	  });
	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Indicator")) {
			auto* indicators = engine.getPtr<Indicators>();
			if (indicators->objectExists(message->text)) {
				bot.getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {
				init_indicator.label = message->text;
				init_indicator.is_label_initialized = true;

				robot_config.loadFromIni();
				const auto& contracts = robot_config.getContracts();
				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
						contracts.operator->()->begin(), contracts.operator->()->end() );

				auto _ = bot.getApi().sendMessage(
						message->chat->id, "Select the ticker for Indicator", false, 0, keyboard_select);
				current_message_id = _->messageId;
				user_activity[message->chat->username] = Event::addIndicator_Ticker;
			}
		}
	  });
	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addIndicator_Ticker) {
			const auto& contracts = robot_config.getContracts();
			for (auto curr = contracts.operator->()->begin(),
						 end = contracts.operator->()->end();
				 curr!=end; ++curr) {
				if (StringTools::startsWith(query->data, curr->second.joint_name)) {
					init_indicator.ticker = curr->second.ticker;
					init_indicator.is_ticker_initialized = true;

					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{},
							init_indicator.trade_sides.begin(), init_indicator.trade_sides.end() );

					auto _ = bot.getApi().sendMessage(
							query->message->chat->id, "Select the Trade Side for Indicator (for CB click any)", false, 0, keyboard_select);
					current_message_id = _->messageId;
					user_activity[query->message->chat->username] = Event::addIndicator_TradeSide;
					break;
				}
			}
		}
	  });
	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addIndicator_TradeSide) {
            if (query->data == "from Tez to Token") {
                init_indicator.trade_side = "SellXTZBuyToken";
                init_indicator.is_trade_side_initialized = true;
            }
            else if (query->data == "from Token to Tez") {
                init_indicator.trade_side = "BuyXTZSellToken";
                init_indicator.is_trade_side_initialized = true;
            }
            else {
                return;
            }

			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{},
					init_indicator.modifiers.begin(), init_indicator.modifiers.end() );

			auto _ = bot.getApi().sendMessage(
					query->message->chat->id, "Select a modifier for the Indicator", false, 0, keyboard_select);
			current_message_id = _->messageId;
			user_activity[query->message->chat->username] = Event::addIndicator_ModifierType;
		}
	  });
	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addIndicator_ModifierType) {
			init_indicator.modifier = query->data;

			if (init_indicator.modifier != "none") {
				ForceReply::Ptr keyboard_reply(new ForceReply);
				auto _ = bot.getApi().sendMessage(query->message->chat->id,
						"Type in a new Modifier value", false, 0, keyboard_reply);
				current_message_id = _->messageId;
				user_activity[query->message->chat->username] = Event::addIndicator_ModifierValue;
			}
			else {
				init_indicator.is_modifier_initialized = true;
				bot.getApi().sendMessage(query->message->chat->id, processEvent(Event::addIndicator));
			}
		}
	  });
	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Type in a new Modifier value") &&
				user_activity[message->chat->username] == Event::addIndicator_ModifierValue) {
			init_indicator.modifier_value = types::Value(message->text);
			init_indicator.is_modifier_initialized = true;
			bot.getApi().sendMessage(message->chat->id, processEvent(Event::addIndicator));
		}
	  });
  }
  void UI::initRemoveIndicator(){
	  bot.getEvents().onCommand("remove_indicator", [this](Message::Ptr message) {
		auto* indicators = engine.getPtr<Indicators>();
		if (indicators->getByLabel()->empty()) {
			bot.getApi().sendMessage(message->chat->id, "No indicators yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
					indicators->getByLabel()->begin(), indicators->getByLabel()->end() );

			auto _ = bot.getApi().sendMessage(message->chat->id,
					"Select the Indicator to remove", false, 0, keyboard_select);
			current_message_id = _->messageId;
			user_activity[message->chat->username] = Event::removeIndicator;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::removeIndicator) {
			auto* indicators = engine.getPtr<Indicators>();
			if (indicators->objectExists(query->data)) {
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::removeIndicator, query->data));
			}
		}
	  });
  }
  void UI::initGetIndicators(){
	  bot.getEvents().onCommand("get_indicators", [this](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, processEvent(Event::getIndicators));
	  });
  }
  void UI::initAddSignal(){
	  bot.getEvents().onCommand("add_signal", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot.getApi().sendMessage(message->chat->id,
				"Label new Signal", false, 0, keyboard_reply);
	  });
	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Signal")) {
			auto* signals = engine.getPtr<Signals>();
			if (signals->objectExists(message->text)) {
				bot.getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {

				init_signal.label = message->text;
				init_signal.is_label_initialized = true;

				auto* indicators = engine.getPtr<Indicators>();

				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
						indicators->getByLabel()->begin(), indicators->getByLabel()->end() );

				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton("finished"));
				keyboard_select->inlineKeyboard.push_back(std::move(row));

				auto _ = bot.getApi().sendMessage(message->chat->id,
						"Select exactly two Indicators to include", false, 0, keyboard_select);
				current_message_id = _->messageId;
				user_activity[message->chat->username] = Event::addSignal;
			}
		}
	  });
	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addSignal) {
			auto* indicators = engine.getPtr<Indicators>();

			//todo: add there should be two only or extend the logic
			if (query->data!="finished") {
				if (indicators->objectExists(query->data)) {
					init_signal.indicator_labels.push_back(query->data); //todo: can be changed for pointers
				}
			}
			else {
				init_signal.is_indicator_labels_initialized = true;

				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{},
						init_signal.signal_types.begin(), init_signal.signal_types.end() );

				auto _ = bot.getApi().sendMessage(query->message->chat->id,
						"Select Signal type", false, 0, keyboard_select);
				current_message_id = _->messageId;
			}
		}
	  });
	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addSignal) {
			for (const auto& s_type : init_signal.signal_types) {
				if (query->data==s_type) {
					init_signal.signal_type = query->data;
					init_signal.is_signal_type_initialized = true;

					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{},
							relations::relation_names.begin(), relations::relation_names.end() );

					auto _ = bot.getApi().sendMessage(query->message->chat->id,
							"Select relation", false, 0, keyboard_select);
					current_message_id = _->messageId;
					break;
				}
			}
		}
	  });
	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addSignal) {
			for (const auto& r : relations::relation_names) {
				if (query->data==r) {
					init_signal.relation = query->data;
					init_signal.is_relation_initialized = true;
					bot.getApi().sendMessage(query->message->chat->id, processEvent(Event::addSignal));
					break;
				}
			}
		}
	  });
  }
  void UI::initRemoveSignal(){
	  bot.getEvents().onCommand("remove_signal", [this](Message::Ptr message) {
		auto* signals = engine.getPtr<Signals>();
		if (signals->getByLabel()->empty()) {
			bot.getApi().sendMessage(message->chat->id, "No signals yet to remove");
		}
		else {

			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
					signals->getByLabel()->begin(), signals->getByLabel()->end() );

			auto _ = bot.getApi().sendMessage(message->chat->id,
					"Select the Signal to remove", false, 0, keyboard_select);
			current_message_id = _->messageId;
			user_activity[message->chat->username] = Event::removeSignal;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::removeSignal) {
			auto* signals = engine.getPtr<Signals>();
			if (signals->objectExists(query->data)) {
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::removeSignal, query->data));
			}
		}
	  });
  }
  void UI::initGetSignals(){
	  bot.getEvents().onCommand("get_signals", [this](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, processEvent(Event::getSignals));
	  });
  }
  void UI::initAddStrategy() {
	  bot.getEvents().onCommand("add_strategy", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot.getApi().sendMessage(message->chat->id,
				"Label new Strategy", false, 0, keyboard_reply);
	  });

	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Strategy")) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(message->text)) {
				bot.getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {

				init_strategy.label = message->text;
				init_strategy.is_label_initialized = true;

				auto* rules = engine.getPtr<Rules>();

				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
						rules->getByLabel()->begin(), rules->getByLabel()->end() );

				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton("finished"));
				keyboard_select->inlineKeyboard.push_back(std::move(row));

				auto _ = bot.getApi().sendMessage(message->chat->id,
						"Select the Rule(s) to include", false, 0, keyboard_select);
				current_message_id = _->messageId;
				user_activity[message->chat->username] = Event::addStrategy;
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addStrategy) {
			auto* rules = engine.getPtr<Rules>();
			if (query->data!="finished") {
				if (rules->objectExists(query->data)) {
					init_strategy.rules_labels.push_back(query->data); //todo: can be changed for pointers
				}
			}
			else {
				init_strategy.is_rule_labels_initialized = true;
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::addStrategy, std::to_string(query->message->chat->id))); //todo: to_string???? - change that!!!
			}
		}
	  });
  }
  void UI::initRemoveStrategy() {
	  bot.getEvents().onCommand("remove_strategy", [this](Message::Ptr message) {
		auto* strategies = engine.getPtr<Strategies>();
		if (strategies->getByLabel()->empty()) {
			bot.getApi().sendMessage(message->chat->id, "No strategies yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
					strategies->getByLabel()->begin(), strategies->getByLabel()->end() );

			auto _ = bot.getApi().sendMessage(message->chat->id,
					"Select the Strategy to remove", false, 0, keyboard_select);
			current_message_id = _->messageId;
			user_activity[message->chat->username] = Event::removeStrategy;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::removeStrategy) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(query->data)) {
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::removeStrategy, query->data));
			}
		}
	  });
  }
  void UI::initAddRule(){
	  bot.getEvents().onCommand("add_rule", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot.getApi().sendMessage(message->chat->id,
				"Label new Rule", false, 0, keyboard_reply);
	  });

	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Rule")) {
			auto* rules = engine.getPtr<Rules>();
			if (rules->objectExists(message->text)) {
				bot.getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {
				init_rule.label = message->text;
				init_rule.is_label_initialized = true;

				InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
				const auto& contracts = robot_config.getContracts();
				for (auto curr = contracts.operator->()->begin(),
							 end = contracts.operator->()->end();
					 curr!=end; ++curr) {
					if (auto dex = curr->second.ticker.find('-'); dex != curr->second.ticker.npos) {
						continue;
					}
					std::vector<InlineKeyboardButton::Ptr> row;
					row.push_back(makeInlineCheckButton(curr->second.joint_name));
					keyboard_select->inlineKeyboard.push_back(std::move(row));
				}
				auto _ = bot.getApi().sendMessage(
						message->chat->id, "Select the ticker to trade with the Rule", false, 0, keyboard_select);
				current_message_id = _->messageId;
				user_activity[message->chat->username] = Event::addRule_TradingTicker;
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username]==Event::addRule_TradingTicker) {
			const auto& contracts = robot_config.getContracts();
			for (auto curr = contracts.operator->()->begin(),
						 end = contracts.operator->()->end();
				 curr!=end; ++curr) {
				if (StringTools::startsWith(query->data, curr->second.joint_name)) {
					init_rule.ticker = curr->second.ticker;
					init_rule.is_ticker_initialized = true;

					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{},
							init_rule.trade_sides.begin(), init_rule.trade_sides.end() );

					auto _ = bot.getApi().sendMessage(
							query->message->chat->id, "Select the Trade Side for Ticker to trade (for CB click any)", false, 0,keyboard_select);
					current_message_id = _->messageId;
					user_activity[query->message->chat->username] = Event::addRule_TradeSide;
					break;
				}
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addRule_TradeSide) {
			if (query->data == "from Tez to Token") {
				init_rule.quipuswap_trade_side = "SellXTZBuyToken";
				init_rule.is_quipuswap_trade_side_initialized = true;
			}
			else if (query->data == "from Token to Tez") {
				init_rule.quipuswap_trade_side = "BuyXTZSellToken";
				init_rule.is_quipuswap_trade_side_initialized = true;
			}
			else {
				return;
			}
			auto* signals = engine.getPtr<Signals>();
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
					signals->getByLabel()->begin(), signals->getByLabel()->end() );

			auto _ = bot.getApi().sendMessage(query->message->chat->id,
					"Select Signal", false, 0, keyboard_select);
			current_message_id = _->messageId;
			user_activity[query->message->chat->username] = Event::addRule_SignalLabel;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addRule_SignalLabel) {
			auto* signals = engine.getPtr<Signals>();
			if (signals->objectExists(query->data)) {
				init_rule.signal_label = query->data;
				init_rule.is_signal_label_initialized = true;

				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{},
						init_rule.signal_values.begin(), init_rule.signal_values.end() );

				auto _ = bot.getApi().sendMessage(query->message->chat->id,
						"Please provide required signal value", false, 0, keyboard_select);
				current_message_id = _->messageId;
				user_activity[query->message->chat->username] = Event::addRule_SignalValue;
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::addRule_SignalValue) {
			init_rule.signal_value = query->data;
			init_rule.is_signal_value_initialized = true;

			init_rule.rule_type = init_rule.rule_type[0]; //todo: CHANGE THIS!!!
			init_rule.is_rule_type_initialized = true;

			init_rule.position_side = init_rule.position_sides[0]; //todo: CHANGE THIS!!!
			init_rule.is_position_side_initialized = true;

			init_rule.order_quantity = 42; //todo: CHANGE THIS!!!
			init_rule.is_order_quantity_initialized = true;

			init_rule.trade_type = init_rule.trade_types[0]; //todo: CHANGE THIS!!!
			init_rule.is_trade_type_initialized = true;

			init_rule.order_type = init_rule.order_types[0]; //todo: CHANGE THIS!!!
			init_rule.is_order_type_initialized = true;

			bot.getApi().sendMessage(query->message->chat->id, processEvent(Event::addRule));
		}
	  });


  }
  void UI::initRemoveRule(){
	  bot.getEvents().onCommand("remove_rule", [this](Message::Ptr message) {
		auto* rules = engine.getPtr<Rules>();
		if (rules->getByLabel()->empty()) {
			bot.getApi().sendMessage(message->chat->id, "No rules yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
					rules->getByLabel()->begin(), rules->getByLabel()->end() );

			auto _ = bot.getApi().sendMessage(message->chat->id,
					"Select the Rule to remove", false, 0, keyboard_select);
			current_message_id = _->messageId;
			user_activity[message->chat->username] = Event::removeRule;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::removeRule) {
			auto* rules = engine.getPtr<Rules>();
			if (rules->objectExists(query->data)) {
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::removeRule, query->data));
			}
		}
	  });
  }
  void UI::initGetRules(){
	  bot.getEvents().onCommand("get_rules", [this](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, processEvent(Event::getRules));
	  });

  }
  void UI::initStopStrategy(){
	  bot.getEvents().onCommand("stop_strategy", [this](Message::Ptr message) {
		auto* strategies = engine.getPtr<Strategies>();
		if (strategies->getByLabel()->empty()) {
			bot.getApi().sendMessage(message->chat->id, "No Strategy to stop");
		}
		else {

			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
					strategies->getByLabel()->begin(), strategies->getByLabel()->end() );

			auto _ = bot.getApi().sendMessage(message->chat->id,
					"Select Strategy to stop", false, 0, keyboard_select);
			current_message_id = _->messageId;
			user_activity[message->chat->username] = Event::stopStrategy;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::stopStrategy) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(query->data)) {
				bot.getApi().sendMessage(query->message->chat->id, processEvent(Event::stopStrategy, query->data));
			}
		}
	  });
  }
  void UI::initStartStrategy(){
	  bot.getEvents().onCommand("start_strategy", [this](Message::Ptr message) {
		auto* strategies = engine.getPtr<Strategies>();
		if (strategies->getByLabel()->empty()) {
			bot.getApi().sendMessage(message->chat->id, "No Strategy to start");
		}
		else {

			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},
					strategies->getByLabel()->begin(), strategies->getByLabel()->end() );

			auto _ = bot.getApi().sendMessage(message->chat->id,
					"Select Strategy to start", false, 0, keyboard_select);
			current_message_id = _->messageId;
			user_activity[message->chat->username] = Event::startStrategy;

		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (query->message->messageId == current_message_id && user_activity[query->message->chat->username] == Event::startStrategy) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(query->data)) {
				bot.getApi().sendMessage(query->message->chat->id, processEvent(Event::startStrategy, query->data));
			}
		}
	  });
  }
  void UI::initStartOperations(){
//	  bot.getApi().sendMessage(chat_id, processEvent(Event::startOperations));
  }
  void UI::initStopOperations(){
//	  bot.getApi().sendMessage(chat_id, processEvent(Event::stopOperations));
  }
  void UI::initStartUI(){
	  bot.getEvents().onCommand("start", [this](Message::Ptr message) {
		processEvent(Event::startOperations);
		types::String greetings = "Hi ";
		greetings += message->chat->firstName;
		greetings += "!";
		bot.getApi().sendMessage(message->chat->id, greetings);
	  });
  }
  void UI::initStopUI(){
/*
	  bot.getEvents().onCommand("exit", [this]([[maybe_unused]] Message::Ptr message) {
		//todo: here the strategy must be stopped, not the bot
		processEvent(Event::stopOperations);
		bot.getApi().sendMessage(message->chat->id, "it was a stop for a strategy");
	  });
*/
  }
  void UI::initAddContract()
  {
	  bot.getEvents().onCommand("add_contract", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot.getApi().sendMessage(message->chat->id,
				"Label new Contract", false, 0, keyboard_reply);
	  });
	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Contract")) {
			auto contracts = robot_config.getContracts();
			if (auto found = contracts->find(message->text); found!=contracts->end()) {
				bot.getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {
				init_contract.label = message->text;
				init_contract.is_label_initialized = true;
				ForceReply::Ptr keyboard_reply(new ForceReply);
				auto _ = bot.getApi().sendMessage(message->chat->id,
						"Type in ticker at Quipuswap", false, 0, keyboard_reply);
				current_message_id = _->messageId;
				user_activity[message->chat->username] = Event::addContract_TickerQS;
			}
		}
	  });
	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->messageId == current_message_id && message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Type in ticker at Quipuswap") &&
				user_activity[message->chat->username]==Event::addContract_TickerQS) {
			init_contract.ticker_qs = message->text;
			init_contract.is_ticker_qs_initialized = true;

			ForceReply::Ptr keyboard_reply(new ForceReply);
			auto _ = bot.getApi().sendMessage(message->chat->id,
					"Type in ticker at Coinbase", false, 0, keyboard_reply);
			current_message_id = _->messageId;
			user_activity[message->chat->username] = Event::addContract_TickerCB;
		}
	  });
	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->messageId == current_message_id && message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Type in ticker at Coinbase") && user_activity[message->chat->username]==Event::addContract_TickerCB) {
			init_contract.ticker_cb = message->text;
			init_contract.is_ticker_cb_initialized = true;

			bot.getApi().sendMessage(message->chat->id, processEvent(Event::addContract));
		}
	  });
  }
  void UI::initGetContracts() {
	  bot.getEvents().onCommand("get_contracts", [this](Message::Ptr message) {
		auto result = processEvent(Event::getContracts);
		bot.getApi().sendMessage(message->chat->id, result);
	  });
  }

  const std::unordered_map<
		  Event,
		  String (UI::*)(const types::String&)
  > UI::EVENT_HANDLER = {
		  {Event::help, &UI::help},
		  {Event::addIndicator, &UI::addIndicator},
		  {Event::removeIndicator, &UI::removeIndicator},
		  {Event::getIndicators, &UI::getIndicators},
		  {Event::addSignal, &UI::addSignal},
		  {Event::removeSignal, &UI::removeSignal},
		  {Event::getSignals, &UI::getSignals},
		  {Event::addStrategy, &UI::addStrategy},
		  {Event::addRule, &UI::addRule},
		  {Event::removeRule, &UI::removeRule},
		  {Event::getRules, &UI::getRules},
		  {Event::removeStrategy, &UI::removeStrategy},
		  {Event::stopStrategy, &UI::stopStrategy},
		  {Event::startStrategy, &UI::startStrategy},
		  {Event::startOperations, &UI::startOperations},
		  {Event::stopOperations, &UI::stopOperations},
		  {Event::startUI, &UI::startUI},
		  {Event::stopUI, &UI::stopUI},
		  {Event::addContract, &UI::addContract},
		  {Event::getContracts, &UI::getContracts},
  };

  String UI::processEvent (Event event, const types::String& input) {
	  if (auto event_exists = EVENT_HANDLER.find(event); event_exists != EVENT_HANDLER.end()) {
		  return (this->*EVENT_HANDLER.at(event))(input);
	  }
	  else return String{};
  }
  String UI::help([[maybe_unused]] const types::String& input){
	  types::String result = R"(
1) Create an Indicator, that holds and updates Market Data for a ticker, either Quipuswap or Coinbase.
2) Create a Signal, that compares two Indicators or an Indicator and a threshold.
3) Create a Rule, that determines what to do in the market once Signal triggers action.
4) Create a Strategy, that may hold several Rules, helping to organize complex behaviour for the Robot.
5) Each strategy should be started separately. Each strategy can be either stopped or started.
)";
	  return result;
  }
  String UI::addIndicator([[maybe_unused]] const types::String& input){
	  if (not init_indicator.isInitialized()) return "Indicator is not initialized"s;

//	  using _K = Timestamp<Minutes>;
//	  using _V = Quote<types::Value, Minutes>;
//	  using _MarketDataContainer = SingleThreadedLimitedSizeMap<_K, _V>;

	  if (init_indicator.modifier == "none") {
		  Indicator indicator (
				  init_indicator.label,
				  init_indicator.ticker,
				  init_indicator.trade_side);
		  engine.addTradingObject(std::move(indicator));
		  init_indicator.clear();
	  }
	  else {
		  //todo: type deduction must aligned with Indicator type - ammend it later
		  Modifier<types::Value> modifier (init_indicator.label);
		  modifier.setModifierValue(std::move(init_indicator.modifier_value));

		  Indicator indicator (
				  init_indicator.label,
				  init_indicator.ticker,
				  init_indicator.trade_side,
				  modifier.getModifierMethod(init_indicator.modifier)
		  );

		  engine.addTradingObject(std::move(modifier));
		  engine.addTradingObject(std::move(indicator));
		  init_indicator.clear();
	  }
	  return "Indicator successfully added"s;
  }
  String UI::removeIndicator([[maybe_unused]] const types::String& input){
	  return "WIP - Removing Indicator";
  }
  String UI::getIndicators([[maybe_unused]] const types::String& input){
	  auto* indicators = engine.getPtr<Indicators>();
	  if (indicators->getByLabel()->empty()) return "No indicators yet"s;
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
  String UI::addSignal([[maybe_unused]] const types::String& input){
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
  String UI::removeSignal([[maybe_unused]] const types::String& input){
	  return "WIP - Removing Signal";
  }
  String UI::getSignals([[maybe_unused]] const types::String& input){
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
  String UI::addStrategy([[maybe_unused]] const types::String& input){
	  if (not init_strategy.isInitialized()) return "Strategy is not initialized"s;

	  Strategy strategy (init_strategy.label,
			  engine.getPtr<Indicators>(),
			  engine.getPtr<Signals>(),
			  engine.getPtr<Rules>(),
			  types::fromChars(input)
	  );
	  auto* rules = engine.getPtr<Rules>();
	  for (const auto& rule_label : init_strategy.rules_labels) {
		  if (rules->objectExists(rule_label)) {
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
  String UI::removeStrategy([[maybe_unused]] const types::String& input){
	  return "WIP - Removing Strategy"s;
  }
  String UI::addRule([[maybe_unused]] const types::String& input){
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
  String UI::removeRule([[maybe_unused]] const types::String& input){
	  return "WIP - Removing Rule";
  }
  String UI::getRules([[maybe_unused]] const types::String& input){
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
  String UI::stopStrategy([[maybe_unused]] const types::String& input){
	  engine.deactivateStrategy(input);

	  String result = "Strategy with Label ";
	  result += input;
	  result += " is offline";
	  return result;
  }
  String UI::startStrategy([[maybe_unused]] const types::String& input){
	  engine.activateStrategy(input);

	  String result = "Strategy with Label ";
	  result += input;
	  result += " is online";
	  return result;
  }
  String UI::startOperations([[maybe_unused]] const types::String& input){
	  return "WIP - Starting the Operations";
  }
  String UI::stopOperations([[maybe_unused]] const types::String& input){
	  return "WIP - Stopping the Operations";
  }
  String UI::startUI([[maybe_unused]] const types::String& input){
	  return "WIP - Start the UI";
  }
  String UI::stopUI([[maybe_unused]] const types::String& input){
	  return "WIP - Stop the UI";
  }
  String UI::addContract([[maybe_unused]] const types::String& input){
	  if (not init_contract.isInitialized()) return "Contract is not initialized"s;

	  config::ContractInfo ci;
	  ci.name = init_contract.label;
	  ci.ticker_cb = init_contract.ticker_cb;
	  ci.ticker_qs = init_contract.ticker_qs;

	  robot_config.updateIni(std::move(ci));
	  init_contract.clear();
	  return "Contract list was updated"s;
  }
  String UI::getContracts([[maybe_unused]] const types::String& input){
	  std::stringstream ss;
	  robot_config.loadFromIni();
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

  void UI::addCommand (String name, String description) {

	  BotCommand::Ptr new_command(new BotCommand);
	  new_command->command = std::move(name);
	  if (not description.empty()) new_command->description = std::move(description);

	  bot.getApi().setMyCommands({new_command});

  }
  void UI::getCommands () {
	  auto vectCmd = bot.getApi().getMyCommands();
	  for(auto it_b = vectCmd.begin(), it_e = vectCmd.end(); it_b != it_e; ++it_b) {
		  std::cerr << (*it_b)->command << ": " << (*it_b)->description << '\n';
	  }
  }
  void UI::executeCommand (String name) {
	  if (auto command = commands.find(name); command != commands.end()) {
	  } else {
	  }
  }

  TgBot::InlineKeyboardButton::Ptr UI::makeInlineCheckButton (const types::String& name) const {
	  InlineKeyboardButton::Ptr checkButton(new InlineKeyboardButton);
	  checkButton->text = name;
	  checkButton->callbackData = name;
	  return checkButton;
  }
}//!namespace