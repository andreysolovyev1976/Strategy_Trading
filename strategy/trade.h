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

	types::String TradeTypeToString (const TradeType &type );
	TradeType StringToTradeType (const types::String& type);

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

	types::String OrderTypeToString (const OrderType &type );
	OrderType StringToOrderType (const types::String& type);

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
	  );

	  const TradingContract& getTradingContract () const;
	  const trade_base::OrderQuantity& getQuantity() const;
	  const trade_base::Slippage& getSlippage() const;
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
