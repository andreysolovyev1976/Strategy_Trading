//
// Created by Andrey Solovyev on 15/02/2022.
//

#include "tg_bot_ui.h"
#include "const_values.h"

using namespace TgBot;

namespace tg_bot {

  using namespace std::literals;

  TgBotUI::TgBotUI()
		  : token_ (const_values::TG_BOT_TOKEN)
		  , bot(token_)
  {
	  chat_id_ = const_values::CHAT_ID;
  }

  void TgBotUI::init () {
	  bot.getApi().sendMessage(chat_id_, "Hi Alex!");

	  bot.getEvents().onCommand("start", [this](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, "it was start");
	  });

	  bot.getEvents().onCommand("get_contracts", [this](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, "it was get_contracts");
	  });

	  bot.getEvents().onCommand("set_strategy", [this](Message::Ptr message) {
		bot.getApi().sendMessage(message->chat->id, "it was set_strategy");
	  });

	  bot.getEvents().onCommand("exit", []([[maybe_unused]] Message::Ptr message) {
		return;
	  });

	  is_initialized = true;
  }


  void TgBotUI::run() {
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
		  throw RuntimeError(EXCEPTION_MSG("TG Bot got broken: "s + e.what() + " "));
	  }

  }

  void TgBotUI::addCommand (types::String name, types::String description) {

	  BotCommand::Ptr new_command(new BotCommand);
	  new_command->command = std::move(name);
	  if (not description.empty()) new_command->description = std::move(description);

	  bot.getApi().setMyCommands({new_command});

  }


  void TgBotUI::postData (types::String text) {

	  /*
     * @brief Use this method to send text messages.
     * @param chatId Unique identifier for the target chat.
     * @param text Text of the message to be sent.
     * @param disableWebPagePreview Optional. Disables link previews for links in this message.
     * @param replyToMessageId Optional. If the message is a reply, ID of the original message.
     * @param replyMarkup Optional. Additional interface options. An object for a custom reply keyboard, instructions to hide keyboard or to force a reply from the user.
     * @param parseMode Optional. Set it to "Markdown" or "HTML" if you want Telegram apps to show bold, italic, fixed-width text or inline URLs in your bot's message.
     * @param disableNotification Optional. Sends the message silenty.
     * @return On success, the sent message is returned.

	  Message::Ptr sendMessage(std::int64_t chatId, const std::string& text, bool disableWebPagePreview = false, std::int32_t replyToMessageId = 0,
			  GenericReply::Ptr replyMarkup = std::make_shared<GenericReply>(), const std::string& parseMode = "", bool disableNotification = false) const;
	  */

	  bot.getApi().sendMessage(chat_id_, std::move(text));

  }

  void TgBotUI::getCommands () {
	  auto vectCmd = bot.getApi().getMyCommands();

	  for(auto it_b = vectCmd.begin(), it_e = vectCmd.end(); it_b != it_e; ++it_b) {
		  std::cerr << (*it_b)->command << ": " << (*it_b)->description << '\n';
	  }
  }



  void TgBotUI::executeCommand (types::String name) {
	  if (auto command = commands.find(name); command != commands.end()) {


	  } else {

	  }

  }


}//!namespace