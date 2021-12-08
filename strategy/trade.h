//
// Created by Andrey Solovyev on 03/11/2021.
//

#include "quote.h"
#include "position.h"
#include "trading_contract.h"

#include <iosfwd>


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
	static types::String TradeTypeToString (const TradeType &type ) {
		if      (type.TryAs<trade_type::Enter>()) return "Enter";
		else if (type.TryAs<trade_type::Exit>()) return "Exit";
		else if (type.TryAs<trade_type::StopLoss>()) return "StopLoss";
		else if (type.TryAs<trade_type::TakeProfit>()) return "TakeProfit";
		else if (type.TryAs<trade_type::Chaining>()) return "Chaining";
		else if (type.TryAs<trade_type::Forced>()) return "Forced";
		else return "";
	}

	[[maybe_unused]]
	static TradeType StringToTradeType (const types::String& type)   {
		if      (type == "Enter")return TradeType{trade_type::Enter{}};
		else if (type == "Exit") return TradeType{trade_type::Exit{}};
		else if (type == "StopLoss") return TradeType{trade_type::StopLoss{}};
		else if (type == "TakeProfit") return TradeType{trade_type::TakeProfit{}};
		else if (type == "Chaining") return TradeType{trade_type::Chaining{}};
		else if (type == "Forced") return TradeType{trade_type::Forced{}};
		else return TradeType{};
	};


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
	static types::String OrderTypeToString (const OrderType &type ) {
		if      (type.TryAs<order_type::Limited>()) return "Limited";
		else if (type.TryAs<order_type::Market>()) return "Market";
		else if (type.TryAs<order_type::FillOrKill>()) return "FillOrKill";
		else return "";
	}

	[[maybe_unused]]
	static OrderType StringToOrderType (const types::String& type)   {
		if      (type == "Limited")return OrderType{order_type::Limited{}};
		else if (type == "Market") return OrderType{order_type::Market{}};
		else if (type == "FillOrKill") return OrderType{order_type::FillOrKill{}};
		else return OrderType{};
	};


	using OrderQuantity = types::Value;

  }//namespace

  class Trade final {
  public:
  	Trade () = default;
  	//todo: impement ctor as a variadic template
	  Trade(
	  		const Ticker &ticker,
	  		const trade_base::OrderQuantity &quantity,
	  		const PositionSide &side,
	  		const trade_base::TradeType &trade_type,
	  		const trade_base::OrderType &order_type)
	  		: ticker_(ticker)
	  		, quantity_(quantity)
	  		, side_(side)
	  		, trade_type_(trade_type)
	  		, order_type_(order_type)
	  		{}

	  const Ticker& getTicker() const {return ticker_;}
	  const trade_base::OrderQuantity& getQuantity() const {return quantity_;}
//	  const Quote& getExecutedPrice() const;
	  const PositionSide& getSide() const;
	  const trade_base::TradeType& getTradeType() const;
	  const trade_base::OrderType& getOrderType() const;

//	  void setPrice (Quote price) {price_ = std::move(price);}

  private:
	  bool is_implemented_ = false;
	  Ticker ticker_;
	  types::Value quantity_;
//	  Quote price_;
	  PositionSide side_;
	  trade_base::TradeType trade_type_;
	  trade_base::OrderType order_type_;
  };

}//!namespace




#endif //STRATEGY_TRADING_TRADE_H
