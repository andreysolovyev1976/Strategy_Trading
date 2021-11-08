//
// Created by Andrey Solovyev on 06/11/2021.
//

#pragma once

#include "value.h"
#include "object_type.h"
#include "trading_contract.h"
#include "timestamp.h"

#ifndef STRATEGY_TRADING_QUOTE_H
#define STRATEGY_TRADING_QUOTE_H

namespace algo {

namespace trade_base {

  namespace trade_side_type {
    struct Long {};
    struct Short {};
  }

  using TradeSideBase = std::variant<
  		std::monostate,
  		trade_side_type::Long,
  		trade_side_type::Short
  		>;
  struct TradeSide final : public types::ObjectType<TradeSideBase> {};

  namespace trade_type {
    struct Enter {};
    struct Exit {};
    struct StopLoss {};
    struct TakeProfit {};
    struct Chaining {};
    struct MarginCall {};
  }

  using TradeTypeBase =
  		std::variant<
  		std::monostate,
  		trade_type::Enter,
  		trade_type::Exit,
  		trade_type::StopLoss,
  		trade_type::TakeProfit,
  		trade_type::Chaining,
  		trade_type::MarginCall
  		>;

  struct TradeType final : public types::ObjectType<TradeTypeBase> {};

  using Ticker = types::String;

  struct TradingContract {
  	Ticker ticker;
  	types::String id_at_chain;
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

  namespace quote_type {
    struct Bid {};
    struct Offer {};
    struct Middle {};
    struct Best {};
  }

  using QuoteTypeBase =
  		std::variant<
  		std::monostate,
  		quote_type::Bid,
  		quote_type::Offer,
  		quote_type::Middle,
  		quote_type::Best
  		>;

}//!namespace

struct Quote final : public types::ObjectType<trade_base::QuoteTypeBase> {
	timestamp::Timestamp timestamp;
	types::Value value;
	Quote(timestamp::Timestamp timestamp, types::Value value);
};

bool operator==(const Quote& lhs, const Quote& rhs);
bool operator!=(const Quote& lhs, const Quote& rhs);
bool operator< (const Quote& lhs, const Quote& rhs);
bool operator> (const Quote& lhs, const Quote& rhs);
bool operator<=(const Quote& lhs, const Quote& rhs);
bool operator>=(const Quote& lhs, const Quote& rhs);
std::ostream& operator<<(std::ostream& os, const Quote& quote);

}//!namespace

#endif //STRATEGY_TRADING_QUOTE_H
