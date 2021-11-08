//
// Created by Andrey Solovyev on 03/11/2021.
//

#include <iosfwd>

#include "quote.h"

#ifndef STRATEGY_TRADING_TRADE_H
#define STRATEGY_TRADING_TRADE_H

namespace algo {

  class Trade final {
  public:
	  Trade(trade_base::Ticker ticker, types::Value quantity, Quote price, trade_base::TradeSide side, trade_base::TradeType trade, trade_base::OrderType order);

	  const trade_base::Ticker& getTicker() const;
	  const types::Value& getQuantity() const;
	  const Quote& getPrice() const;
	  const trade_base::TradeSide& getSide() const;
	  const trade_base::TradeType& getTrade() const;
	  const trade_base::OrderType& getOrder() const;
  private:
  	bool is_implemented_ = false;
	  trade_base::Ticker ticker;
	  types::Value quantity;
	  Quote price;
	  trade_base::TradeSide side;
	  trade_base::TradeType trade;
	  trade_base::OrderType order;
  };

}//!namespace




#endif //STRATEGY_TRADING_TRADE_H
