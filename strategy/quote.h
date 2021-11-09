//
// Created by Andrey Solovyev on 06/11/2021.
//

#pragma once

#include "object_type.h"
#include "trading_contract.h"
#include "timestamp.h"

#ifndef STRATEGY_TRADING_QUOTE_H
#define STRATEGY_TRADING_QUOTE_H

namespace algo {

  namespace quote_base {

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

  struct Quote final : public types::ObjectType<quote_base::QuoteTypeBase> {
	  //todo: divide in classes for Type and Object
	  // make it templated for Value and int (plain vanilla int for Signal)
	  Quote() = default;
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
