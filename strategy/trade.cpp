//
// Created by Andrey Solovyev on 04/11/2021.
//

#include "trade.h"

namespace algo {
  namespace trade_base {

	types::String TradeTypeToString (const TradeType &type ) {
		if      (type.TryAs<trade_type::Enter>()) return "Enter";
		else if (type.TryAs<trade_type::Exit>()) return "Exit";
		else if (type.TryAs<trade_type::StopLoss>()) return "StopLoss";
		else if (type.TryAs<trade_type::TakeProfit>()) return "TakeProfit";
		else if (type.TryAs<trade_type::Chaining>()) return "Chaining";
		else if (type.TryAs<trade_type::Forced>()) return "Forced";
		else return "";
	}

	TradeType StringToTradeType (const types::String& type)   {
		if      (type == "Enter")return TradeType{trade_type::Enter{}};
		else if (type == "Exit") return TradeType{trade_type::Exit{}};
		else if (type == "StopLoss") return TradeType{trade_type::StopLoss{}};
		else if (type == "TakeProfit") return TradeType{trade_type::TakeProfit{}};
		else if (type == "Chaining") return TradeType{trade_type::Chaining{}};
		else if (type == "Forced") return TradeType{trade_type::Forced{}};
		else return TradeType{};
	}


	types::String OrderTypeToString (const OrderType &type ) {
		if      (type.TryAs<order_type::Limited>()) return "Limited";
		else if (type.TryAs<order_type::Market>()) return "Market";
		else if (type.TryAs<order_type::FillOrKill>()) return "FillOrKill";
		else return "";
	}

	OrderType StringToOrderType (const types::String& type)   {
		if      (type == "Limited")return OrderType{order_type::Limited{}};
		else if (type == "Market") return OrderType{order_type::Market{}};
		else if (type == "FillOrKill") return OrderType{order_type::FillOrKill{}};
		else return OrderType{};
	}

  }//!namespace

  Trade::Trade(
		  const TradingContract &trading_contract,
		  const trade_base::OrderQuantity &quantity,
		  const trade_base::Slippage &slippage
  )
		  : trading_contract_(trading_contract)
		  , quantity_(quantity)
		  , slippage_(slippage)
  {}

  const TradingContract& Trade::getTradingContract() const {return trading_contract_;}
  const trade_base::OrderQuantity& Trade::getQuantity() const {return quantity_;}
  const trade_base::Slippage& Trade::getSlippage() const {return slippage_;}

}//!namespace


