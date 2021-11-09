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

    using OrderQuantity = types::Value;

  }//namespace

  class Trade final {
  public:
  	Trade(Ticker ticker, types::Value quantity, Quote price, PositionSide side, trade_base::TradeType trade_type, trade_base::OrderType order_type);

	  const Ticker& getTicker() const;
	  const types::Value& getQuantity() const;
	  const Quote& getExecutedPrice() const;
	  const PositionSide& getSide() const;
	  const trade_base::TradeType& getTradeType() const;
	  const trade_base::OrderType& getOrderType() const;
  private:
  	bool is_implemented_ = false;
	  Ticker ticker;
	  types::Value quantity;
	  Quote price;
	  PositionSide side;
	  trade_base::TradeType trade_type;
	  trade_base::OrderType order_type;
  };

}//!namespace




#endif //STRATEGY_TRADING_TRADE_H
