//
// Created by Andrey Solovyev on 15/02/2022.
//

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

  template <typename C>
  UI<C>::UI(C* controller)
		  : c_ptr(std::shared_ptr<C>(controller, [](auto*){/* empty deleter */}))
		  , bot(std::shared_ptr<TgBot::Bot>(c_ptr->getBotPtr(), [](auto*){/* empty deleter */}))
		  , robot_config (c_ptr->getConfig())
		  , engine (c_ptr->getEngine())
		  , ctors (c_ptr->getCtors())
  {
	  for (Event curr = Event::begin, last = Event::end; curr != last; curr = nextEvent(curr)){
		  initCommand(curr);
	  }
	  is_ui_initialized = true;
  }//!ctor

  template <typename C>
  void UI<C>::run() {
	  if (not is_ui_initialized) {
		  throw LogicError(EXCEPTION_MSG("TG Bot is not initialized "));
	  }
	  try {
		  bot->getApi().deleteWebhook();

		  TgLongPoll longPoll(*bot);
		  while (true) {
			  longPoll.start();
		  }
	  }
	  catch (std::exception& e) {
		  if (chat_id.has_value())
			  bot->getApi().sendMessage(chat_id.value(), EXCEPTION_MSG("TG Bot caught exception: "s+e.what()+" "));
#ifdef CERR_OVER_BOT
		  else std::cerr << e.what() << '\n';
#endif
#ifdef BOT_OVER_CERR
		  throw RuntimeError(EXCEPTION_MSG("TG Bot caught exception: "s + e.what() + " "));
#endif
	  }
  }//!func

  template <typename C>
  const std::unordered_map<
		  Event,
		  void (UI<C>::*)()
  > UI<C>::INIT = {
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
  };
  template <typename C>
  void UI<C>::initCommand (Event event) {
	  if (auto event_exists = INIT.find(event); event_exists != INIT.end()) {
		  (this->*INIT.at(event))();
	  }
  }
  template <typename C>
  void UI<C>::initHelp(){
	  bot->getEvents().onCommand("help", [this](Message::Ptr message) {
		chat_id = message->chat->id;
		bot->getApi().sendMessage(message->chat->id, c_ptr->processEvent(Event::help));
	  });
  }
  template <typename C>
  void UI<C>::initAddIndicator(){
	  bot->getEvents().onCommand("add_indicator", [this](Message::Ptr message) {
		chat_id = message->chat->id;
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot->getApi().sendMessage(message->chat->id,
				"Label new Indicator", false, 0, keyboard_reply);
	  });
	  bot->getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (checkMessageForResponse(message, "Label new Indicator")) {
			auto* indicators = engine.getPtr<Indicators>();
			if (indicators->objectExists(message->text)) {
				bot->getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {
				ctors.init_indicator.label = message->text;
				ctors.init_indicator.is_label_initialized = true;
				robot_config.loadFromIni();
				const auto& contracts = robot_config.getContracts();
				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, contracts.operator->()->begin(), contracts.operator->()->end() );
				sendRequestForInput (message->chat, keyboard_select, "Select the ticker for Indicator", Event::addIndicator_Ticker);
			}
		}
	  });
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addIndicator_Ticker)) {
			const auto& contracts = robot_config.getContracts();
			for (auto curr = contracts.operator->()->begin(),
						 end = contracts.operator->()->end();
				 curr!=end; ++curr) {
				if (StringTools::startsWith(query->data, curr->second.joint_name)) {
					ctors.init_indicator.ticker = curr->second.ticker;
					ctors.init_indicator.is_ticker_initialized = true;
					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{}, ctors.init_indicator.trade_sides.begin(), ctors.init_indicator.trade_sides.end() );
					sendRequestForInput (query->message->chat, keyboard_select, "Select the Trade Side for Indicator (for CB click any)", Event::addIndicator_TradeSide);
					break;
				}
			}
		}
	  });
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query,  Event::addIndicator_TradeSide)) {
			if (query->data == "from Tez to Token") {
				ctors.init_indicator.trade_side = "SellXTZBuyToken";
				ctors.init_indicator.is_trade_side_initialized = true;
			}
			else if (query->data == "from Token to Tez") {
				ctors.init_indicator.trade_side = "BuyXTZSellToken";
				ctors.init_indicator.is_trade_side_initialized = true;
			}
			else {
				return;
			}
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{},ctors.init_indicator.modifiers.begin(), ctors.init_indicator.modifiers.end() );
			sendRequestForInput (query->message->chat,keyboard_select,"Select a modifier for the Indicator",Event::addIndicator_ModifierType);
		}
	  });
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addIndicator_ModifierType)) {
			ctors.init_indicator.modifier = query->data;
			if (ctors.init_indicator.modifier != "none") {
				ForceReply::Ptr keyboard_reply(new ForceReply);
				sendRequestForInput (query->message->chat, keyboard_reply, "Type in a new Modifier value", Event::addIndicator_ModifierValue);
			}
			else {
				ctors.init_indicator.is_modifier_initialized = true;
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::addIndicator));
			}
		}
	  });
	  bot->getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (checkMessageForEvent(message, "Type in a new Modifier value", Event::addIndicator_ModifierValue)) {
			ctors.init_indicator.modifier_value = types::Value(message->text);
			ctors.init_indicator.is_modifier_initialized = true;
			bot->getApi().sendMessage(message->chat->id, c_ptr->processEvent(Event::addIndicator));
		}
	  });
  }
  template <typename C>
  void UI<C>::initRemoveIndicator(){
	  bot->getEvents().onCommand("remove_indicator", [this](Message::Ptr message) {
		auto* indicators = engine.getPtr<Indicators>();
		if (indicators->getByLabel()->empty()) {
			bot->getApi().sendMessage(message->chat->id, "No indicators yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, indicators->getByLabel()->begin(), indicators->getByLabel()->end() );
			sendRequestForInput (message->chat, keyboard_select, "Select the Indicator to remove", Event::removeIndicator);
		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::removeIndicator)) {
			auto* indicators = engine.getPtr<Indicators>();
			if (indicators->objectExists(query->data)) {
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::removeIndicator, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initGetIndicators(){
	  bot->getEvents().onCommand("get_indicators", [this](Message::Ptr message) {
		bot->getApi().sendMessage(message->chat->id, c_ptr->processEvent(Event::getIndicators));
	  });
  }
  template <typename C>
  void UI<C>::initAddSignal(){
	  bot->getEvents().onCommand("add_signal", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		sendRequestForInput (message->chat, keyboard_reply, "Label new Signal");
	  });
	  bot->getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (checkMessageForResponse(message, "Label new Signal")) {
			auto* signals = engine.getPtr<Signals>();
			if (signals->objectExists(message->text)) {
				bot->getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {

				ctors.init_signal.label = message->text;
				ctors.init_signal.is_label_initialized = true;

				auto* indicators = engine.getPtr<Indicators>();

				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, indicators->getByLabel()->begin(), indicators->getByLabel()->end() );

				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton("finished"));
				keyboard_select->inlineKeyboard.push_back(std::move(row));
				sendRequestForInput (message->chat, keyboard_select, "Select exactly two Indicators to include", Event::addSignal);
			}
		}
	  });
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addSignal)) {
			auto* indicators = engine.getPtr<Indicators>();

			//todo: add there should be two only or extend the logic
			if (query->data!="finished") {
				if (indicators->objectExists(query->data)) {
					ctors.init_signal.indicator_labels.push_back(query->data); //todo: can be changed for pointers
				}
			}
			else {
				ctors.init_signal.is_indicator_labels_initialized = true;
				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{}, ctors.init_signal.signal_types.begin(), ctors.init_signal.signal_types.end() );
				sendRequestForInput (query->message->chat, keyboard_select, "Select Signal type", Event::addSignal);
			}
		}
	  });
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addSignal)) {
			for (const auto& s_type : ctors.init_signal.signal_types) {
				if (query->data==s_type) {
					ctors.init_signal.signal_type = query->data;
					ctors.init_signal.is_signal_type_initialized = true;
					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{}, relations::relation_names.begin(), relations::relation_names.end() );
					sendRequestForInput (query->message->chat, keyboard_select, "Select relation", Event::addSignal);
					break;
				}
			}
		}
	  });
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addSignal)) {
			for (const auto& r : relations::relation_names) {
				if (query->data==r) {
					ctors.init_signal.relation = query->data;
					ctors.init_signal.is_relation_initialized = true;
					bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::addSignal));
					break;
				}
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initRemoveSignal(){
	  bot->getEvents().onCommand("remove_signal", [this](Message::Ptr message) {
		auto* signals = engine.getPtr<Signals>();
		if (signals->getByLabel()->empty()) {
			bot->getApi().sendMessage(message->chat->id, "No signals yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, signals->getByLabel()->begin(), signals->getByLabel()->end() );
			sendRequestForInput (message->chat, keyboard_select, "Select the Signal to remove", Event::removeSignal);
		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::removeSignal)) {
			auto* signals = engine.getPtr<Signals>();
			if (signals->objectExists(query->data)) {
				bot->getApi().sendMessage(query->message->chat->id,c_ptr->processEvent(Event::removeSignal, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initGetSignals(){
	  bot->getEvents().onCommand("get_signals", [this](Message::Ptr message) {
		bot->getApi().sendMessage(message->chat->id, c_ptr->processEvent(Event::getSignals));
	  });
  }
  template <typename C>
  void UI<C>::initAddStrategy() {
	  bot->getEvents().onCommand("add_strategy", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		sendRequestForInput (message->chat, keyboard_reply, "Label new Strategy");
	  });

	  bot->getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (checkMessageForResponse(message, "Label new Strategy")) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(message->text)) {
				bot->getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {

				ctors.init_strategy.label = message->text;
				ctors.init_strategy.is_label_initialized = true;

				auto* rules = engine.getPtr<Rules>();

				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, rules->getByLabel()->begin(), rules->getByLabel()->end() );

				std::vector<InlineKeyboardButton::Ptr> row;
				row.push_back(makeInlineCheckButton("finished"));
				keyboard_select->inlineKeyboard.push_back(std::move(row));
				sendRequestForInput (message->chat, keyboard_select, "Select the Rule(s) to include", Event::addStrategy);
			}
		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addStrategy)) {
			auto* rules = engine.getPtr<Rules>();
			if (query->data!="finished") {
				if (rules->objectExists(query->data)) {
					ctors.init_strategy.rules_labels.push_back(query->data); //todo: can be changed for pointers
				}
			}
			else {
				ctors.init_strategy.is_rule_labels_initialized = true;
				bot->getApi().sendMessage(query->message->chat->id,
						c_ptr->processEvent(Event::addStrategy, std::to_string(query->message->chat->id))); //todo: to_string???? - change that!!!
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initRemoveStrategy() {
	  bot->getEvents().onCommand("remove_strategy", [this](Message::Ptr message) {
		auto* strategies = engine.getPtr<Strategies>();
		if (strategies->getByLabel()->empty()) {
			bot->getApi().sendMessage(message->chat->id, "No strategies yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, strategies->getByLabel()->begin(), strategies->getByLabel()->end() );
			sendRequestForInput (message->chat, keyboard_select, "Select the Strategy to remove", Event::removeStrategy);
		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::removeStrategy)) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(query->data)) {
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::removeStrategy, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initAddRule(){
	  bot->getEvents().onCommand("add_rule", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		sendRequestForInput (message->chat, keyboard_reply, "Label new Rule");
	  });

	  bot->getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (checkMessageForResponse(message, "Label new Rule")) {
			auto* rules = engine.getPtr<Rules>();
			if (rules->objectExists(message->text)) {
				bot->getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {
				ctors.init_rule.label = message->text;
				ctors.init_rule.is_label_initialized = true;

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
				sendRequestForInput (message->chat, keyboard_select, "Select the ticker to trade with the Rule", Event::addRule_TradingTicker);
			}
		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addRule_TradingTicker)) {
			const auto& contracts = robot_config.getContracts();
			for (auto curr = contracts.operator->()->begin(),
						 end = contracts.operator->()->end();
				 curr!=end; ++curr) {
				if (StringTools::startsWith(query->data, curr->second.joint_name)) {
					ctors.init_rule.ticker = curr->second.ticker;
					ctors.init_rule.is_ticker_initialized = true;
					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{},ctors.init_rule.trade_sides.begin(), ctors.init_rule.trade_sides.end() );
					sendRequestForInput (query->message->chat, keyboard_select, "Select the Trade Side for Ticker to trade (for CB click any)", Event::addRule_TradeSide);
					break;
				}
			}
		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addRule_TradeSide)) {
			if (query->data == "from Tez to Token") {
				ctors.init_rule.quipuswap_trade_side = "SellXTZBuyToken";
				ctors.init_rule.is_quipuswap_trade_side_initialized = true;
			}
			else if (query->data == "from Token to Tez") {
				ctors.init_rule.quipuswap_trade_side = "BuyXTZSellToken";
				ctors.init_rule.is_quipuswap_trade_side_initialized = true;
			}
			else {
				return;
			}
			auto* signals = engine.getPtr<Signals>();
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},signals->getByLabel()->begin(), signals->getByLabel()->end() );
			sendRequestForInput (query->message->chat, keyboard_select, "Select Signal", Event::addRule_SignalLabel);

		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addRule_SignalLabel)) {
			auto* signals = engine.getPtr<Signals>();
			if (signals->objectExists(query->data)) {
				ctors.init_rule.signal_label = query->data;
				ctors.init_rule.is_signal_label_initialized = true;

				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{},ctors.init_rule.signal_values.begin(), ctors.init_rule.signal_values.end() );
				sendRequestForInput (query->message->chat, keyboard_select, "Please provide required signal value", Event::addRule_SignalValue);
			}
		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addRule_SignalValue)) {
			ctors.init_rule.signal_value = query->data;
			ctors.init_rule.is_signal_value_initialized = true;

			ctors.init_rule.rule_type = ctors.init_rule.rule_type[0]; //todo: CHANGE THIS!!!
			ctors.init_rule.is_rule_type_initialized = true;

			ctors.init_rule.position_side = ctors.init_rule.position_sides[0]; //todo: CHANGE THIS!!!
			ctors.init_rule.is_position_side_initialized = true;

			ctors.init_rule.order_quantity = 42; //todo: CHANGE THIS!!!
			ctors.init_rule.is_order_quantity_initialized = true;

			ctors.init_rule.trade_type = ctors.init_rule.trade_types[0]; //todo: CHANGE THIS!!!
			ctors.init_rule.is_trade_type_initialized = true;

			ctors.init_rule.order_type = ctors.init_rule.order_types[0]; //todo: CHANGE THIS!!!
			ctors.init_rule.is_order_type_initialized = true;

			bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::addRule));
		}
	  });
  }
  template <typename C>
  void UI<C>::initRemoveRule(){
	  bot->getEvents().onCommand("remove_rule", [this](Message::Ptr message) {
		auto* rules = engine.getPtr<Rules>();
		if (rules->getByLabel()->empty()) {
			bot->getApi().sendMessage(message->chat->id, "No rules yet to remove");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, rules->getByLabel()->begin(), rules->getByLabel()->end() );
			sendRequestForInput (message->chat, keyboard_select, "Select the Rule to remove", Event::removeRule);
		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::removeRule)) {
			auto* rules = engine.getPtr<Rules>();
			if (rules->objectExists(query->data)) {
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::removeRule, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initGetRules(){
	  bot->getEvents().onCommand("get_rules", [this](Message::Ptr message) {
		bot->getApi().sendMessage(message->chat->id, c_ptr->processEvent(Event::getRules));
	  });

  }
  template <typename C>
  void UI<C>::initStopStrategy(){
	  bot->getEvents().onCommand("stop_strategy", [this](Message::Ptr message) {
		auto* strategies = engine.getPtr<Strategies>();
		if (strategies->getByLabel()->empty()) {
			bot->getApi().sendMessage(message->chat->id, "No Strategy to stop");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},strategies->getByLabel()->begin(), strategies->getByLabel()->end() );
			sendRequestForInput (message->chat, keyboard_select, "Select Strategy to stop", Event::stopStrategy);
		}
	  });
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::stopStrategy)) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(query->data)) {
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::stopStrategy, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initStartStrategy(){
	  bot->getEvents().onCommand("start_strategy", [this](Message::Ptr message) {
		auto* strategies = engine.getPtr<Strategies>();
		if (strategies->getByLabel()->empty()) {
			bot->getApi().sendMessage(message->chat->id, "No Strategy to start");
		}
		else {
			InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, strategies->getByLabel()->begin(), strategies->getByLabel()->end() );
			sendRequestForInput (message->chat, keyboard_select, "Select Strategy to start", Event::startStrategy);
		}
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::startStrategy)) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(query->data)) {
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::startStrategy, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initStartOperations(){
//	  bot->getApi().sendMessage(chat_id, controller_ptr->processEvent(Event::startOperations));
  }
  template <typename C>
  void UI<C>::initStopOperations(){
//	  bot->getApi().sendMessage(chat_id, controller_ptr->processEvent(Event::stopOperations));
  }
  template <typename C>
  void UI<C>::initStartUI(){
	  bot->getEvents().onCommand("start", [this](Message::Ptr message) {
		c_ptr->processEvent(Event::startOperations);
		types::String greetings = "Hi ";
		greetings += message->chat->firstName;
		greetings += "!";
		bot->getApi().sendMessage(message->chat->id, greetings);
	  });
  }
  template <typename C>
  void UI<C>::initStopUI(){
/*
	  bot->getEvents().onCommand("exit", [this]([[maybe_unused]] Message::Ptr message) {
		//todo: here the strategy must be stopped, not the bot
		controller_ptr->processEvent(Event::stopOperations);
		bot->getApi().sendMessage(message->chat->id, "it was a stop for a strategy");
	  });
*/
  }
  template <typename C>
  void UI<C>::initAddContract()
  {
	  bot->getEvents().onCommand("add_contract", [this](Message::Ptr message) {
		ForceReply::Ptr keyboard_reply(new ForceReply);
		bot->getApi().sendMessage(message->chat->id,
				"Label new Contract", false, 0, keyboard_reply);
	  });
	  bot->getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (message->replyToMessage && StringTools::startsWith(message->replyToMessage->text,
				"Label new Contract")) {
			auto contracts = robot_config.getContracts();
			if (auto found = contracts->find(message->text); found!=contracts->end()) {
				bot->getApi().sendMessage(message->chat->id, "This Label already exists");
			}
			else {
				ctors.init_contract.label = message->text;
				ctors.init_contract.is_label_initialized = true;
				ForceReply::Ptr keyboard_reply(new ForceReply);
				sendRequestForInput (message->chat, keyboard_reply, "Type in ticker at Quipuswap", Event::addContract_TickerQS);
			}
		}
	  });
	  bot->getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (checkMessageForEvent(message, "Type in ticker at Quipuswap", Event::addContract_TickerCB)) {
			ctors.init_contract.ticker_qs = message->text;
			ctors.init_contract.is_ticker_qs_initialized = true;

			ForceReply::Ptr keyboard_reply(new ForceReply);
			sendRequestForInput (message->chat, keyboard_reply, "Type in ticker at Coinbase", Event::addContract_TickerCB);
		}
	  });
	  bot->getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (checkMessageForEvent(message, "Type in ticker at Coinbase", Event::addContract_TickerCB)) {
			ctors.init_contract.ticker_cb = message->text;
			ctors.init_contract.is_ticker_cb_initialized = true;
			bot->getApi().sendMessage(message->chat->id, c_ptr->processEvent(Event::addContract));
		}
	  });
  }
  template <typename C>
  void UI<C>::initGetContracts() {
	  bot->getEvents().onCommand("get_contracts", [this](Message::Ptr message) {
		auto result = c_ptr->processEvent(Event::getContracts);
		bot->getApi().sendMessage(message->chat->id, result);
	  });
  }

  template <typename C>
  TgBot::InlineKeyboardButton::Ptr UI<C>::makeInlineCheckButton (const types::String& name) const {
	  InlineKeyboardButton::Ptr checkButton(new InlineKeyboardButton);
	  checkButton->text = name;
	  checkButton->callbackData = name;
	  return checkButton;
  }

  template <typename C>
  template <typename ConstIter, typename Type>
  TgBot::InlineKeyboardMarkup::Ptr UI<C>::makeInlineKeyboard ([[maybe_unused]] Type type, ConstIter b, ConstIter e) const {
	  TgBot::InlineKeyboardMarkup::Ptr keyboard_select(new TgBot::InlineKeyboardMarkup);
	  if constexpr (std::is_same_v<Type, UI::Vec>) {
		  for (auto curr = b; curr!=e; ++curr) {
			  std::vector<TgBot::InlineKeyboardButton::Ptr> row;
			  row.push_back(makeInlineCheckButton(*curr));
			  keyboard_select->inlineKeyboard.push_back(std::move(row));
		  }
	  }
	  else if constexpr (std::is_same_v<Type, UI::Map>) {
		  for (auto curr = b; curr!=e; ++curr) {
			  std::vector<TgBot::InlineKeyboardButton::Ptr> row;
			  row.push_back(makeInlineCheckButton(curr->first));
			  keyboard_select->inlineKeyboard.push_back(std::move(row));
		  }
	  }
	  return keyboard_select;
  }

  template <typename C>
  template <typename Keyboard>
  void UI<C>::sendRequestForInput (TgBot::Chat::Ptr chat, Keyboard keyboard, types::String msg, Event event) {
	  auto _ = bot->getApi().sendMessage(chat->id, msg, false, 0, keyboard);
	  current_message_id[chat->username] = _->messageId;
	  if (event != Event::begin) {
		  user_activity[chat->username] = event;
	  }

  }

  template <typename C>
  bool UI<C>::checkQueryForEvent (TgBot::CallbackQuery::Ptr query, Event event) {
	  return
			  query->message->messageId == current_message_id[query->message->chat->username] &&
					  user_activity[query->message->chat->username] == event;
  }

  template <typename C>
  bool UI<C>::checkMessageForEvent (TgBot::Message::Ptr message, types::String msg, Event event) {
	  return
			  message->messageId == current_message_id[message->chat->username] &&
					  message->replyToMessage &&
					  StringTools::startsWith(message->replyToMessage->text, msg) &&
					  user_activity[message->chat->username]==event;
  }

  template <typename C>
  bool UI<C>::checkMessageForResponse (TgBot::Message::Ptr message, types::String msg) {
	  return
			  message->replyToMessage &&
					  StringTools::startsWith(message->replyToMessage->text, msg);
  }

}//!namespace