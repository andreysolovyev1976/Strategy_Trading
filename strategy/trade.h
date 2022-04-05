//
// Created by Andrey Solovyev on 03/11/2021.
//

#include "quote.h"
#include "position.h"
#include "trading_contract.h"

#ifndef STRATEGY_TRADING_TRADE_H
#define STRATEGY_TRADING_TRADE_H

namespace algo {

  namespace trade_base {

	namespace trade_type {
	  struct Enter {};
	  struct Exit {};
	  struct StopLoss {};
	  struct TakeProfit {};
	  struct Chaining {};
	  struct Forced {};
	}

	using TradeTypeBase =
	std::variant<
			std::monostate,
			trade_type::Enter,
			trade_type::Exit,
			trade_type::StopLoss,
			trade_type::TakeProfit,
			trade_type::Chaining,
			trade_type::Forced
	>;

	struct TradeType final : public types::ObjectType<TradeTypeBase> {};

	[[maybe_unused]]
	inline
	types::String TradeTypeToString (const TradeType &type ) {
		if      (type.TryAs<trade_type::Enter>()) return "Enter";
		else if (type.TryAs<trade_type::Exit>()) return "Exit";
		else if (type.TryAs<trade_type::StopLoss>()) return "StopLoss";
		else if (type.TryAs<trade_type::TakeProfit>()) return "TakeProfit";
		else if (type.TryAs<trade_type::Chaining>()) return "Chaining";
		else if (type.TryAs<trade_type::Forced>()) return "Forced";
		else return "";
	}

	[[maybe_unused]]
	inline
	TradeType StringToTradeType (const types::String& type)   {
		if      (type == "Enter")return TradeType{trade_type::Enter{}};
		else if (type == "Exit") return TradeType{trade_type::Exit{}};
		else if (type == "StopLoss") return TradeType{trade_type::StopLoss{}};
		else if (type == "TakeProfit") return TradeType{trade_type::TakeProfit{}};
		else if (type == "Chaining") return TradeType{trade_type::Chaining{}};
		else if (type == "Forced") return TradeType{trade_type::Forced{}};
		else return TradeType{};
	}

	namespace order_type {
	  struct Limited {};
	  struct Market {};
	  struct FillOrKill {};
	}

	using OrderTypeBase =
	std::variant<
			std::monostate,
			order_type::Limited,
			order_type::Market,
			order_type::FillOrKill
	>;

	struct OrderType final : public types::ObjectType<OrderTypeBase> {};

	[[maybe_unused]]
	inline
	types::String OrderTypeToString (const OrderType &type ) {
		if      (type.TryAs<order_type::Limited>()) return "Limited";
		else if (type.TryAs<order_type::Market>()) return "Market";
		else if (type.TryAs<order_type::FillOrKill>()) return "FillOrKill";
		else return "";
	}

	[[maybe_unused]]
	inline
	OrderType StringToOrderType (const types::String& type)   {
		if      (type == "Limited")return OrderType{order_type::Limited{}};
		else if (type == "Market") return OrderType{order_type::Market{}};
		else if (type == "FillOrKill") return OrderType{order_type::FillOrKill{}};
		else return OrderType{};
	}

	using OrderQuantity = types::Value;
	using Slippage = types::Value;

  }//namespace

  class Trade final {
  public:
	  //todo: impement ctor as a variadic template
	  explicit Trade(
			  const TradingContract &trading_contract,
			  const trade_base::OrderQuantity &quantity,
			  const trade_base::Slippage &slippage
//	  		const PositionSide &side,
//	  		const trade_base::TradeType &trade_type,
//	  		const trade_base::OrderType &order_type
	  )
			  : trading_contract_(trading_contract)
			  , quantity_(quantity)
			  , slippage_(slippage)
	  {}

	  const TradingContract& getTradingContract() const {return trading_contract_;}
	  const trade_base::OrderQuantity& getQuantity() const {return quantity_;}
	  const trade_base::Slippage& getSlippage() const {return slippage_;}
//	  const Quote& getExecutedPrice() const;
//	  const PositionSide& getSide() const;
//	  const trade_base::TradeType& getTradeType() const;
//	  const trade_base::OrderType& getOrderType() const;

//	  void setPrice (Quote price) {price_ = std::move(price);}

  private:

	  const TradingContract& trading_contract_;
	  trade_base::OrderQuantity quantity_;
	  trade_base::Slippage slippage_;
//	  Quote price_;
//	  PositionSide side_;
//	  trade_base::TradeType trade_type_;
//	  trade_base::OrderType order_type_;
  };

}//!namespace


#endif //STRATEGY_TRADING_TRADE_H
