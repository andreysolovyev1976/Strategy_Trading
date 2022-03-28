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

  Controller::Controller()
		  : token_ (const_values::TG_BOT_TOKEN)
		  , bot(token_)
		  , robot_config(config::RobotConfig::getInstance(const_values::CONFIG_FILENAME))
  {
	  for (Event curr = Event::begin, last = Event::end; curr != last; curr = nextEvent(curr)){
		  if (curr == Event::begin) continue;
		  initCommand(curr);
	  }
	  is_initialized = true;
  }//!ctor

  void Controller::run() {
	  std::thread bot_thread(&Controller::runBot, this);
	  bot_thread.detach();

	  std::thread engine_thread(&Engine::iterateOverStrategies, std::ref(this->engine), std::ref(active_strategies));
	  engine_thread.detach();

	  while (true) {}
  }

  void Controller::runBot() {
	  if (not is_initialized){
		  throw LogicError(EXCEPTION_MSG("TG Bot is not initialized "));
	  }
	  try {
		  bot.getApi().deleteWebhook();

		  TgLongPoll longPoll(bot);
		  while (true) {
			  longPoll.start();
		  }
	  } catch (std::exception& e) {
//		  bot.getApi().sendMessage(chat_id, EXCEPTION_MSG("TG Bot caught exception: "s + e.what() + " "));
		  throw RuntimeError(EXCEPTION_MSG("TG Bot caught exception: "s + e.what() + " "));
	  }
  }

  const std::unordered_map<
		  Event,
		  void (Controller::*)()
  > Controller::INIT = {
		  {Event::addIndicator, &Controller::initAddIndicator},
		  {Event::removeIndicator, &Controller::initRemoveIndicator},
		  {Event::getIndicators, &Controller::initGetIndicators},
		  {Event::addSignal, &Controller::initAddSignal},
		  {Event::removeSignal, &Controller::initRemoveSignal},
		  {Event::getSignals, &Controller::initGetSignals},
		  {Event::addStrategy, &Controller::initAddStrategy},
		  {Event::removeStrategy, &Controller::initRemoveStrategy},
		  {Event::addRule, &Controller::initAddRule},
		  {Event::removeRule, &Controller::initRemoveRule},
		  {Event::getRules, &Controller::initGetRules},
		  {Event::stopStrategy, &Controller::initStopStrategy},
		  {Event::startStrategy, &Controller::initStartStrategy},
		  {Event::startOperations, &Controller::initStartOperations},
		  {Event::stopOperations, &Controller::initStopOperations},
		  {Event::startUI, &Controller::initStartUI},
		  {Event::stopUI, &Controller::initStopUI},
		  {Event::getContracts, &Controller::initGetContracts},
		  {Event::initRobot, &Controller::initRobot},
  };
  void Controller::initCommand (Event event) {
	  (this->*INIT.at(event))();
  }
  void Controller::initRobot() {
	  robot_config.loadFromIni();

	  /*
	  config.printIniFile(std::cerr);
	  config.printSettings(std::cerr);

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
  void Controller::initAddIndicator(){

	  bot.getEvents().onCommand("add_indicator", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot.getApi().sendMessage(message->chat->id,
				"Label new Indicator", false, 0, keyboard_reply);
	  });

	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Indicator")) {
			auto* indicators = engine.getPtr<Indicators>();
			if (auto found = indicators->getPtr(message->text); found) {
				bot.getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {
				init_indicator.label = message->text;
				init_indicator.is_label_initialized = true;

				InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
				const auto& contracts = robot_config.getContracts();
				for (auto curr = contracts.operator->()->begin(),
							 end = contracts.operator->()->end();
					 curr!=end; ++curr) {
					std::vector<InlineKeyboardButton::Ptr> row;
					row.push_back(makeInlineCheckButton(curr->second.joint_name));
					keyboard_select->inlineKeyboard.push_back(std::move(row));
				}
				bot.getApi().sendMessage(
						message->chat->id, "Select the ticker for Indicator", false, 0, keyboard_select);
				user_ui_activity[message->chat->username] = Event::addIndicator;
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::addIndicator) {
			const auto& contracts = robot_config.getContracts();
			for (auto curr = contracts.operator->()->begin(),
						 end = contracts.operator->()->end();
				 curr!=end; ++curr) {
				if (StringTools::startsWith(query->data, curr->second.joint_name)) {
					init_indicator.ticker = curr->second.ticker;
					init_indicator.is_ticker_initialized = true;
					bot.getApi().sendMessage(query->message->chat->id, processEvent(Event::addIndicator));
					break;
				}
			}
		}
	  });
  }
  void Controller::initRemoveIndicator(){
	  bot.getEvents().onCommand("remove_indicator", [this](Message::Ptr message) {
		auto* indicators = engine.getPtr<Indicators>();
		if (indicators->getByLabel()->Empty()) {
			bot.getApi().sendMessage(message->chat->id, "No indicators yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
			for (const auto&[_label, _] : *(indicators->getByLabel())) {
				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton(_label));
				keyboard_select->inlineKeyboard.push_back(std::move(row));
			}
			bot.getApi().sendMessage(message->chat->id,
					"Select the Indicator to remove", false, 0, keyboard_select);
			user_ui_activity[message->chat->username] = Event::removeIndicator;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::removeIndicator) {
			auto* indicators = engine.getPtr<Indicators>();
			if (auto found = indicators->getPtr(query->data); found) {
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::removeIndicator, query->data));
			}
		}
	  });
  }
  void Controller::initGetIndicators(){
	  bot.getEvents().onCommand("get_indicators", [this](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, processEvent(Event::getIndicators));
	  });
  }
  void Controller::initAddSignal(){
	  bot.getEvents().onCommand("add_signal", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot.getApi().sendMessage(message->chat->id,
				"Label new Signal", false, 0, keyboard_reply);
	  });

	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Signal")) {
			auto* signals = engine.getPtr<Signals>();
			if (auto found = signals->getPtr(message->text); found) {
				bot.getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {

				init_signal.label = message->text;
				init_signal.is_label_initialized = true;

				auto* indicators = engine.getPtr<Indicators>();
				InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
				for (const auto&[_label, _] : *indicators->getByLabel()) {
					std::vector<InlineKeyboardButton::Ptr> row;
					row.push_back(makeInlineCheckButton(_label));
					keyboard_select->inlineKeyboard.push_back(std::move(row));
				}
				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton("finished"));
				keyboard_select->inlineKeyboard.push_back(std::move(row));

				auto _ = bot.getApi().sendMessage(message->chat->id,
						"Select the Indicator(s) to include", false, 0, keyboard_select);
				user_ui_activity[message->chat->username] = Event::addSignal;
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::addSignal) {
			auto* indicators = engine.getPtr<Indicators>();

			//todo: add there should be two only or extend the logic
			if (query->data!="finished") {
				if (auto found = indicators->getPtr(query->data); found) {
					init_signal.indicator_labels.push_back(query->data); //todo: can be changed for pointers
				}
			}
			else {
				init_signal.is_indicator_labels_initialized = true;

				InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
				for (const auto& s_type : init_signal.signal_types) {
					std::vector<InlineKeyboardButton::Ptr> row;
					row.push_back(makeInlineCheckButton(s_type));
					keyboard_select->inlineKeyboard.push_back(std::move(row));
				}
				bot.getApi().sendMessage(query->message->chat->id,
						"Select Signal type", false, 0, keyboard_select);
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::addSignal) {
			for (const auto& s_type : init_signal.signal_types) {
				if (query->data==s_type) {
					init_signal.signal_type = query->data;
					init_signal.is_signal_type_initialized = true;

					InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
					for (const auto& r : relations::relation_names) {
						std::vector<InlineKeyboardButton::Ptr> row;
						row.push_back(makeInlineCheckButton(r));
						keyboard_select->inlineKeyboard.push_back(std::move(row));
					}
					bot.getApi().sendMessage(query->message->chat->id,
							"Select relation", false, 0, keyboard_select);
					break;
				}
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::addSignal) {
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
  void Controller::initRemoveSignal(){
	  bot.getEvents().onCommand("remove_signal", [this](Message::Ptr message) {
		auto* signals = engine.getPtr<Signals>();
		if (signals->getByLabel()->Empty()) {
			bot.getApi().sendMessage(message->chat->id, "No signals yet to remove");
		}
		else {

			InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
			for (const auto&[_label, _] : *(signals->getByLabel()) ) {
				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton(_label));
				keyboard_select->inlineKeyboard.push_back(std::move(row));
			}
			bot.getApi().sendMessage(message->chat->id,
					"Select the Signal to remove", false, 0, keyboard_select);
			user_ui_activity[message->chat->username] = Event::removeSignal;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::removeSignal) {
			auto* signals = engine.getPtr<Signals>();
			if (auto found = signals->getPtr(query->data); found) {
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::removeSignal, query->data));
			}
		}
	  });
  }
  void Controller::initGetSignals(){
	  bot.getEvents().onCommand("get_signals", [this](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, processEvent(Event::getSignals));
	  });
  }
  void Controller::initAddStrategy() {
	  bot.getEvents().onCommand("add_strategy", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot.getApi().sendMessage(message->chat->id,
				"Label new Strategy", false, 0, keyboard_reply);
	  });

	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Strategy")) {
			auto* strategies = engine.getPtr<Strategies>();
			if (auto found = strategies->getPtr(message->text); found) {
				bot.getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {

				init_strategy.label = message->text;
				init_strategy.is_label_initialized = true;

				auto* rules = engine.getPtr<Rules>();
				InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
				for (const auto&[_label, _] : *(rules->getByLabel())) {
					std::vector<InlineKeyboardButton::Ptr> row;
					row.push_back(makeInlineCheckButton(_label));
					keyboard_select->inlineKeyboard.push_back(std::move(row));
				}
				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton("finished"));
				keyboard_select->inlineKeyboard.push_back(std::move(row));

				bot.getApi().sendMessage(message->chat->id,
						"Select the Rule(s) to include", false, 0, keyboard_select);
				user_ui_activity[message->chat->username] = Event::addStrategy;
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::addStrategy) {
			auto* rules = engine.getPtr<Rules>();
			if (query->data!="finished") {
				if (auto found = rules->getPtr(query->data); found) {
					init_strategy.rules_labels.push_back(query->data); //todo: can be changed for pointers
				}
			}
			else {
				init_strategy.is_rule_labels_initialized = true;
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::addStrategy, query->data));
			}
		}
	  });
  }
  void Controller::initRemoveStrategy() {
	  bot.getEvents().onCommand("remove_strategy", [this](Message::Ptr message) {
		auto* strategies = engine.getPtr<Strategies>();
		if (strategies->getByLabel()->Empty()) {
			bot.getApi().sendMessage(message->chat->id, "No strategies yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
			for (const auto&[_label, _] : *(strategies->getByLabel())) {
				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton(_label));
				keyboard_select->inlineKeyboard.push_back(std::move(row));
			}
			bot.getApi().sendMessage(message->chat->id,
					"Select the Strategy to remove", false, 0, keyboard_select);
			user_ui_activity[message->chat->username] = Event::removeStrategy;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::removeStrategy) {
			auto* strategies = engine.getPtr<Strategies>();
			if (auto found = strategies->getPtr(query->data); found) {
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::removeStrategy, query->data));
			}
		}
	  });
  }
  void Controller::initAddRule(){
	  bot.getEvents().onCommand("add_rule", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot.getApi().sendMessage(message->chat->id,
				"Label new Rule", false, 0, keyboard_reply);
	  });

	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Rule")) {
			auto* rules = engine.getPtr<Rules>();
			if (auto found = rules->getPtr(message->text); found) {
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
					std::vector<InlineKeyboardButton::Ptr> row;
					row.push_back(makeInlineCheckButton(curr->second.joint_name));
					keyboard_select->inlineKeyboard.push_back(std::move(row));
				}
				auto _ = bot.getApi().sendMessage(
						message->chat->id, "Select the ticker to trade with the Rule", false, 0, keyboard_select);
				user_ui_activity[message->chat->username] = Event::addRule;
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::addRule) {
			const auto& contracts = robot_config.getContracts();
			for (auto curr = contracts.operator->()->begin(),
						 end = contracts.operator->()->end();
				 curr!=end; ++curr) {
				if (StringTools::startsWith(query->data, curr->second.joint_name)) {
					init_rule.ticker = curr->second.ticker;
					init_rule.is_ticker_initialized = true;

					InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
					auto* signals = engine.getPtr<Signals>();
					for (const auto& [label, _] : *(signals->getByLabel()) ) {
						std::vector<InlineKeyboardButton::Ptr> row;
						row.push_back(makeInlineCheckButton(label));
						keyboard_select->inlineKeyboard.push_back(std::move(row));
					}
					bot.getApi().sendMessage(query->message->chat->id,
							"Select Signal", false, 0, keyboard_select);
					break;
				}
			}
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::addRule) {
			auto* signals = engine.getPtr<Signals>();
			if (auto found = signals->getPtr(query->data); found) {
				init_rule.signal_label = query->data;
				init_rule.is_signal_label_initialized = true;

				//todo: CHANGE THIS TO TRUE / FALSE / VALUE
				ForceReply::Ptr keyboard_reply(new ForceReply);
				bot.getApi().sendMessage(query->message->chat->id,
						"Please provide required signal value", false, 0, keyboard_reply);
			}
		}
	  });

	  bot.getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Please provide required signal value")) {

			init_rule.signal_value = fromChars(message->text);
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

			bot.getApi().sendMessage(message->chat->id, processEvent(Event::addRule));
		}
	  });
  }
  void Controller::initRemoveRule(){
	  bot.getEvents().onCommand("remove_rule", [this](Message::Ptr message) {
		auto* rules = engine.getPtr<Rules>();
		if (rules->getByLabel()->Empty()) {
			bot.getApi().sendMessage(message->chat->id, "No rules yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
			for (const auto&[_label, _] : *(rules->getByLabel())) {
				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton(_label));
				keyboard_select->inlineKeyboard.push_back(std::move(row));
			}
			auto _ = bot.getApi().sendMessage(message->chat->id,
					"Select the Rule to remove", false, 0, keyboard_select);

			user_ui_activity[message->chat->username] = Event::removeRule;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::removeRule) {
			auto* rules = engine.getPtr<Rules>();
			if (auto found = rules->getPtr(query->data); found) {
				bot.getApi().sendMessage(query->message->chat->id,
						processEvent(Event::removeRule, query->data));
			}
		}
	  });
  }
  void Controller::initGetRules(){
	  bot.getEvents().onCommand("get_rules", [this](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, processEvent(Event::getRules));
	  });

  }
  void Controller::initStopStrategy(){
	  bot.getEvents().onCommand("stop_strategy", [this](Message::Ptr message) {
		auto* strategies = engine.getPtr<Strategies>();
		if (strategies->getByLabel()->Empty()) {
			bot.getApi().sendMessage(message->chat->id, "No Strategy to stop");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
			for (const auto&[label, _] : *(strategies->getByLabel())) {
				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton(label));
				keyboard_select->inlineKeyboard.push_back(std::move(row));
			}
			auto _ = bot.getApi().sendMessage(message->chat->id,
					"Select Strategy to stop", false, 0, keyboard_select);
			user_ui_activity[message->chat->username] = Event::stopStrategy;
		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::stopStrategy) {
			auto* strategies = engine.getPtr<Strategies>();
			if (auto found = strategies->getPtr(query->data); found) {
				bot.getApi().sendMessage(query->message->chat->id, processEvent(Event::stopStrategy, query->data));
			}
		}
	  });
  }
  void Controller::initStartStrategy(){
	  bot.getEvents().onCommand("start_strategy", [this](Message::Ptr message) {
		auto* strategies = engine.getPtr<Strategies>();
		if (strategies->getByLabel()->Empty()) {
			bot.getApi().sendMessage(message->chat->id, "No Strategy to start");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);
			for (const auto&[label, _] : *(strategies->getByLabel())) {
				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton(label));
				keyboard_select->inlineKeyboard.push_back(std::move(row));
			}
			bot.getApi().sendMessage(message->chat->id,
					"Select Strategy to start", false, 0, keyboard_select);
			user_ui_activity[message->chat->username] = Event::startStrategy;

		}
	  });

	  bot.getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (user_ui_activity[query->message->chat->username] == Event::startStrategy) {
			auto* strategies = engine.getPtr<Strategies>();
			if (auto found = strategies->getPtr(query->data); found) {
				bot.getApi().sendMessage(query->message->chat->id, processEvent(Event::startStrategy, query->data));
			}
		}
	  });
  }
  void Controller::initStartOperations(){
//	  bot.getApi().sendMessage(chat_id, processEvent(Event::startOperations));
  }
  void Controller::initStopOperations(){
//	  bot.getApi().sendMessage(chat_id, processEvent(Event::stopOperations));
  }
  void Controller::initStartUI(){
	  bot.getEvents().onCommand("start", [this](Message::Ptr message) {
		processEvent(Event::startOperations);
		types::String greetings = "Hi ";
		greetings += message->chat->firstName;
		greetings += "!";
		bot.getApi().sendMessage(message->chat->id, greetings);
	  });
  }
  void Controller::initStopUI(){
/*
	  bot.getEvents().onCommand("exit", [this]([[maybe_unused]] Message::Ptr message) {
		//todo: here the strategy must be stopped, not the bot
		processEvent(Event::stopOperations);
		bot.getApi().sendMessage(message->chat->id, "it was a stop for a strategy");
	  });
*/
  }
  void Controller::initGetContracts() {
	  bot.getEvents().onCommand("get_contracts", [this](Message::Ptr message) {
		auto result = processEvent(Event::getContracts);
		bot.getApi().sendMessage(message->chat->id, result);
	  });
  }

  const std::unordered_map<
		  Event,
		  String (Controller::*)(const types::String&)
  > Controller::EVENT_HANDLER = {
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
	  return (this->*EVENT_HANDLER.at(event))(input);
  }
  String Controller::addIndicator([[maybe_unused]] const types::String& input){
	  if (not init_indicator.isInitialized()) return "Indicator is not initialized"s;

//	  using _K = Timestamp<Minutes>;
//	  using _V = Quote<types::Value, Minutes>;
//	  using _MarketDataContainer = SingleThreadedLimitedSizeMap<_K, _V>;
	  Indicator indicator (init_indicator.ticker, init_indicator.label);
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
			  init_rule.ticker,
			  init_rule.label,
			  init_rule.rule_type,
			  init_rule.signal_label,
			  init_rule.signal_value,
			  init_rule.position_side,
			  init_rule.order_quantity,
			  init_rule.trade_type,
			  init_rule.order_type,
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
	  engine.deactivateStrategy(input, active_strategies); //todo - make it a separate thread

	  String result = "Strategy with Label ";
	  result += input;
	  result += " is offline";
	  return result;
  }
  String Controller::startStrategy([[maybe_unused]] const types::String& input){
	  engine.activateStrategy(input, active_strategies); //todo - make it a separate thread

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

  void Controller::addCommand (String name, String description) {

	  BotCommand::Ptr new_command(new BotCommand);
	  new_command->command = std::move(name);
	  if (not description.empty()) new_command->description = std::move(description);

	  bot.getApi().setMyCommands({new_command});

  }

  void Controller::getCommands () {
	  auto vectCmd = bot.getApi().getMyCommands();

	  for(auto it_b = vectCmd.begin(), it_e = vectCmd.end(); it_b != it_e; ++it_b) {
		  std::cerr << (*it_b)->command << ": " << (*it_b)->description << '\n';
	  }
  }

  void Controller::executeCommand (String name) {
	  if (auto command = commands.find(name); command != commands.end()) {

	  } else {

	  }

  }


  TgBot::InlineKeyboardButton::Ptr Controller::makeInlineCheckButton (const types::String& name) const {
	  InlineKeyboardButton::Ptr checkButton(new InlineKeyboardButton);
	  checkButton->text = name;
	  checkButton->callbackData = name;
	  return checkButton;
  }


}//!namespace