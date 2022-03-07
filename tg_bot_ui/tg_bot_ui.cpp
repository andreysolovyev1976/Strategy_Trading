//
// Created by Andrey Solovyev on 15/02/2022.
//

#include "tg_bot_ui.h"

using namespace TgBot;

namespace tg_bot {


  TgBotUI::TgBotUI(types::String token)
  : token_ (std::move(token))
  , bot(token_)
  {
	  bot_id_ = bot.getApi().getMe()->id;
		chat_id_ = 442233888;
		bot.getApi().sendMessage(chat_id_, "Hi Alex!");
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

  types::String TgBotUI::makeCommand (const TgBot::BotCommand::Ptr bot_command) {
	  types::String output;
	  output.reserve(bot_command->command.size() + 1);
	  output += '/';
	  output += bot_command->command;
	  return output;
  }
}//!namespace