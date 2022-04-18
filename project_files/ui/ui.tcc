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
		  {Event::setup, &UI::initMainMenu},

		  {Event::setup_Indicators, &UI::initIndicatorMenu},
		  {Event::addIndicator, &UI::initAddIndicator},
		  {Event::removeIndicator, &UI::initRemoveIndicator},
//		  {Event::getIndicators, &UI::initGetIndicators},

		  {Event::setup_Signals, &UI::initSignalMenu},
		  {Event::addSignal, &UI::initAddSignal},
		  {Event::removeSignal, &UI::initRemoveSignal},
//		  {Event::getSignals, &UI::initGetSignals},

		  {Event::setup_Strategies, &UI::initStrategyMenu},
		  {Event::addStrategy, &UI::initAddStrategy},
		  {Event::removeStrategy, &UI::initRemoveStrategy},
//		  {Event::getStrategies, &UI::initGetStrategies},
		  {Event::stopStrategy, &UI::initStopStrategy},
		  {Event::startStrategy, &UI::initStartStrategy},

		  {Event::setup_Rules, &UI::initRuleMenu},
		  {Event::addRule, &UI::initAddRule},
		  {Event::removeRule, &UI::initRemoveRule},
//		  {Event::getRules, &UI::initGetRules},

		  {Event::setup_Contracts, &UI::initContractMenu},
		  {Event::addContract, &UI::initAddContract},
		  {Event::removeContract, &UI::initRemoveContract},
//		  {Event::getContracts, &UI::initGetContracts},

		  {Event::startOperations, &UI::initStartOperations},
		  {Event::stopOperations, &UI::initStopOperations},
		  {Event::startUI, &UI::initStartUI},
		  {Event::stopUI, &UI::initStopUI},
		  {Event::setup, &UI::initMainMenu},
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
  void UI<C>::initMainMenu() {
	  bot->getEvents().onCommand("menu", [this](Message::Ptr message) {
		InlineKeyboardMarkup::Ptr keyboard_select(new InlineKeyboardMarkup);

		size_t size = main_menu.size();
		auto curr = main_menu.begin();
		for (size_t i = 0; i < size; i = i + 2) {
			std::vector<InlineKeyboardButton::Ptr> row;
			for (int j = 0; j != 2; ++j) {
				if (curr == main_menu.end()) break;
				const auto&[trading_object, _] = *curr;
				row.push_back(makeInlineCheckButton(trading_object));
				curr = std::next(curr);
			}
			keyboard_select->inlineKeyboard.push_back(std::move(row));
			if (curr == main_menu.end()) break;
		}
		sendRequestForInput (message->chat, keyboard_select, "Select Trading Object", Event::setup_selectTradingObject);
	  });

	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::setup_selectTradingObject)) {
			updateKeyboard(query);
			if (auto trading_object = main_menu.find(query->data); trading_object!=main_menu.end()) {
				InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Vec{}, trading_object->second.begin(), trading_object->second.end() );
				types::String msg = "Select action for ";
				msg += query->data;
				sendRequestForInput (query->message->chat, keyboard_select, std::move(msg), main_menu_events.at(query->data));
			}
			else {
				bot->getApi().sendMessage(query->message->chat->id, "Unexpected selection");
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initIndicatorMenu() {
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::setup_Indicators)) {
			hideInlineKeyboard(query);
			if (query->data == "Add") {
				appendMessage(query->message, ": Adding...");
				ForceReply::Ptr keyboard_reply(new ForceReply);
				sendRequestForInput (query->message->chat, keyboard_reply, "Label new Indicator");
			}
			else if (query->data == "List") {
				appendMessage(query->message, ": Listing...");
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::getIndicators));
			}
			else if (query->data == "Remove") {
				appendMessage(query->message, ": Removing...");
				auto* indicators = engine.getPtr<Indicators>();
				if (indicators->getByLabel()->empty()) {
					bot->getApi().sendMessage(query->message->chat->id, "No indicators yet to remove");
				}
				else {
					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, indicators->getByLabel()->begin(), indicators->getByLabel()->end() );
					types::String msg = "Before you remove an Indicator, please make sure the Indicator you are deleting is not used at Signals and therefore - Strategies.\n\nSelect the Indicator to remove";
					sendRequestForInput (query->message->chat, keyboard_select, std::move(msg), Event::removeIndicator);
				}
			}
			else {
				bot->getApi().sendMessage(query->message->chat->id, "Unexpected selection");
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initAddIndicator(){
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
			updateKeyboard(query);
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
			updateKeyboard(query);
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
			updateKeyboard(query);
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
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::removeIndicator)) {
			auto* indicators = engine.getPtr<Indicators>();
			if (indicators->objectExists(query->data)) {
				updateKeyboard(query);
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::removeIndicator, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initSignalMenu() {
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::setup_Signals)) {
			hideInlineKeyboard(query);
			if (query->data == "Add") {
				appendMessage(query->message, ": Adding...");
				ForceReply::Ptr keyboard_reply(new ForceReply);
				sendRequestForInput (query->message->chat, keyboard_reply, "Label new Signal");
			}
			else if (query->data == "List") {
				appendMessage(query->message, ": Listing...");
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::getSignals));

			}
			else if (query->data == "Remove") {
				appendMessage(query->message, ": Removing...");
				auto* signals = engine.getPtr<Signals>();
				if (signals->getByLabel()->empty()) {
					bot->getApi().sendMessage(query->message->chat->id, "No signals yet to remove");
				}
				else {
					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, signals->getByLabel()->begin(), signals->getByLabel()->end() );
					types::String msg = "Before you remove a Signal, please make sure the Signal you are deleting is not used at Rules and therefore - Strategies.\n\nSelect the Signal to remove";
					sendRequestForInput (query->message->chat, keyboard_select, std::move(msg), Event::removeSignal);
				}
			}
			else {
				bot->getApi().sendMessage(query->message->chat->id, "Unexpected selection");
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initAddSignal(){
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
			updateKeyboard(query);
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
			updateKeyboard(query);
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
			updateKeyboard(query);
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
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::removeSignal)) {
			auto* signals = engine.getPtr<Signals>();
			if (signals->objectExists(query->data)) {
				updateKeyboard(query);
				bot->getApi().sendMessage(query->message->chat->id,c_ptr->processEvent(Event::removeSignal, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initStrategyMenu() {
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::setup_Strategies)) {
			hideInlineKeyboard(query);
			if (query->data == "Add") {
				appendMessage(query->message, ": Adding...");
				ForceReply::Ptr keyboard_reply(new ForceReply);
				sendRequestForInput (query->message->chat, keyboard_reply, "Label new Strategy");
			}
			else if (query->data == "List") {
				appendMessage(query->message, ": Listing...");
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::getStrategies));
			}
			else if (query->data == "Remove") {
				appendMessage(query->message, ": Removing...");
				auto* strategies = engine.getPtr<Strategies>();
				if (strategies->getByLabel()->empty()) {
					bot->getApi().sendMessage(query->message->chat->id, "No strategies yet to remove");
				}
				else {
					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, strategies->getByLabel()->begin(), strategies->getByLabel()->end() );
					sendRequestForInput (query->message->chat, keyboard_select, "Select the Strategy to remove", Event::removeStrategy);
				}
			}
			else if (query->data == "Start") {
				appendMessage(query->message, ": Selecting target...");
				auto* strategies = engine.getPtr<Strategies>();
				if (strategies->getByLabel()->empty()) {
					bot->getApi().sendMessage(query->message->chat->id, "No Strategy to start");
				}
				else {
					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, strategies->getByLabel()->begin(), strategies->getByLabel()->end() );
					sendRequestForInput (query->message->chat, keyboard_select, "Select Strategy to start", Event::startStrategy);
				}
			}
			else if (query->data == "Stop") {
				appendMessage(query->message, ": Selecting target...");
				auto* strategies = engine.getPtr<Strategies>();
				if (strategies->getByLabel()->empty()) {
					bot->getApi().sendMessage(query->message->chat->id, "No Strategy to stop");
				}
				else {
					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{},strategies->getByLabel()->begin(), strategies->getByLabel()->end() );
					sendRequestForInput (query->message->chat, keyboard_select, "Select Strategy to stop", Event::stopStrategy);
				}
			}
			else {
				bot->getApi().sendMessage(query->message->chat->id, "Unexpected selection");
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initAddStrategy() {
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
				updateKeyboard(query);
				bot->getApi().sendMessage(query->message->chat->id,
						c_ptr->processEvent(Event::addStrategy, std::to_string(query->message->chat->id))); //todo: to_string???? - change that!!!
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initRemoveStrategy() {
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::removeStrategy)) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(query->data)) {
				updateKeyboard(query);
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::removeStrategy, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initStopStrategy(){
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::stopStrategy)) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(query->data)) {
				updateKeyboard(query);
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::stopStrategy, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initStartStrategy(){
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::startStrategy)) {
			auto* strategies = engine.getPtr<Strategies>();
			if (strategies->objectExists(query->data)) {
				updateKeyboard(query);
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::startStrategy, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initRuleMenu() {
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::setup_Rules)) {
			hideInlineKeyboard(query);
			if (query->data == "Add") {
				appendMessage(query->message, ": Adding...");
				ForceReply::Ptr keyboard_reply(new ForceReply);
				sendRequestForInput (query->message->chat, keyboard_reply, "Label new Rule");
			}
			else if (query->data == "List") {
				appendMessage(query->message, ": Listing...");
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::getRules));
			}
			else if (query->data == "Remove") {
				appendMessage(query->message, ": Removing...");
				auto* rules = engine.getPtr<Rules>();
				if (rules->getByLabel()->empty()) {
					bot->getApi().sendMessage(query->message->chat->id, "No rules yet to remove");
				}
				else {
					InlineKeyboardMarkup::Ptr keyboard_select = makeInlineKeyboard(Map{}, rules->getByLabel()->begin(), rules->getByLabel()->end() );
					types::String msg = "Before you remove a Rule, please make sure the Rule you are deleting is not used at Strategies.\n\nSelect the Rule to remove";
					sendRequestForInput (query->message->chat, keyboard_select, std::move(msg), Event::removeRule);
				}
			}
			else {
				bot->getApi().sendMessage(query->message->chat->id, "Unexpected selection");
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initAddRule(){
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
			updateKeyboard(query);
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
			updateKeyboard(query);
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
			updateKeyboard(query);
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
			updateKeyboard(query);
			ctors.init_rule.signal_value = query->data;
			ctors.init_rule.is_signal_value_initialized = true;

			ForceReply::Ptr keyboard_reply(new ForceReply);
			sendRequestForInput(query->message->chat, keyboard_reply, "Enter single trade volume", Event::addRule_OrderQuantity);
		}
	  });
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addRule_OrderQuantity)) {
			updateKeyboard(query);
			ctors.init_rule.order_quantity = query->data;
			ctors.init_rule.is_order_quantity_initialized = true;
			ForceReply::Ptr keyboard_reply(new ForceReply);
			sendRequestForInput(query->message->chat, keyboard_reply, "Enter slippage for a single trade", Event::addRule_Slippage);
		}
	  });
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::addRule_Slippage)) {
			updateKeyboard(query);

			ctors.init_rule.slippage = query->data;
			ctors.init_rule.is_slippage_initialized = true;

			ctors.init_rule.rule_type = ctors.init_rule.rule_type[0]; //todo: CHANGE THIS!!!
			ctors.init_rule.is_rule_type_initialized = true;

			ctors.init_rule.position_side = ctors.init_rule.position_sides[0]; //todo: CHANGE THIS!!!
			ctors.init_rule.is_position_side_initialized = true;

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
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::removeRule)) {
			auto* rules = engine.getPtr<Rules>();
			if (rules->objectExists(query->data)) {
				updateKeyboard(query);
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::removeRule, query->data));
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initContractMenu() {
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::setup_Contracts)) {
			hideInlineKeyboard(query);
			if (query->data == "Add") {
				appendMessage(query->message, ": Adding...");
				ForceReply::Ptr keyboard_reply(new ForceReply);
				sendRequestForInput (query->message->chat, keyboard_reply, "Label new Contract");
			}
			else if (query->data == "List") {
				appendMessage(query->message, ": Listing...");
				bot->getApi().sendMessage(query->message->chat->id, c_ptr->processEvent(Event::getContracts));
			}
			else if (query->data == "Remove") {
				appendMessage(query->message, ": Removing...");

//					types::String msg = "Before you remove a Contract, please make sure the Contract you are deleting is not used at Indicators and therefore - Strategies.\n\nSelect the Contract to remove";
//					sendRequestForInput (query->message->chat, keyboard_select, std::move(msg), Event::removeContract);
			}
			else {
				bot->getApi().sendMessage(query->message->chat->id, "Unexpected selection");
			}
		}
	  });
  }
  template <typename C>
  void UI<C>::initAddContract(){
	  bot->getEvents().onNonCommandMessage([this](Message::Ptr message) {
		if (checkMessageForResponse(message, "Label new Contract")) {
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
		if (checkMessageForEvent(message, "Type in ticker at Quipuswap", Event::addContract_TickerQS)) {
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
  void UI<C>::initRemoveContract() {
	  bot->getEvents().onCallbackQuery([this](CallbackQuery::Ptr query) {
		if (checkQueryForEvent(query, Event::removeContract)) {
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
  void UI<C>::hideInlineKeyboard (TgBot::CallbackQuery::Ptr query) {
  	if (query) {
  		bot->getApi().editMessageReplyMarkup(
				query->message->chat->id,
				query->message->messageId);
	}
  	else {
  		throw InvalidArgumentError(EXCEPTION_MSG("Query Ptr is not valid; "));
  	}
  }
  template <typename C>
  void UI<C>::appendMessage (TgBot::Message::Ptr message, types::String msg) const {
	  if (message) {
		  types::String new_text = message->text;
		  new_text += std::move(msg);

		  bot->getApi().editMessageText (
				  new_text,
				  message->chat->id,
				  message->messageId);
	  }
	  else {
		  throw InvalidArgumentError(EXCEPTION_MSG("Query Ptr is not valid; "));
	  }
  }

  template <typename C>
  void UI<C>::updateKeyboard (TgBot::CallbackQuery::Ptr query) {
	  hideInlineKeyboard(query);
	  types::String amendment = ": ";
	  amendment += query->data;
	  appendMessage(query->message, amendment);
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
			  message->replyToMessage &&
					  message->replyToMessage->messageId == current_message_id[message->chat->username] &&
					  StringTools::startsWith(message->replyToMessage->text, msg) &&
					  user_activity[message->chat->username] == event;
  }
  template <typename C>
  bool UI<C>::checkMessageForResponse (TgBot::Message::Ptr message, types::String msg) {
	  return
			  message->replyToMessage &&
					  StringTools::startsWith(message->replyToMessage->text, msg);
  }

}//!namespace