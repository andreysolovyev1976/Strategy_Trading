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
	  struct SingleQuote {
		  types::Value value;
	  };
	  struct BidAsk {
		  types::Value
				  bid, ask, middle, price;
	  };
	  struct OHLCV {
		  types::Value
				  open, high, low, close, volume;
	  };
	}

	using QuoteTypeBase =
	std::variant<
			std::monostate,
			types::Value,
			quote_type::SingleQuote,
			quote_type::BidAsk,
			quote_type::OHLCV
	>;

	std::ostream& operator << (std::ostream& os, const QuoteTypeBase &data);

  }//!namespace

  using namespace quote_base::quote_type;

  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
  struct Quote final {
	  Quote() = default;

	  Quote(QuoteType v);
	  Quote& operator = (const QuoteType &v);

	  //todo:: generalize this
	  const types::Value& value () const;
	  types::Value& value ();

	  time_::Timestamp<Duration> timestamp;
	  types::ObjectType<quote_base::QuoteTypeBase> value_;
  };


  template <typename QuoteType, typename Duration>
  Quote<QuoteType, Duration>::Quote(QuoteType v) {
  	value_.template emplace<QuoteType>(v);
  }

  template <typename QuoteType, typename Duration>
  Quote<QuoteType, Duration>& Quote<QuoteType, Duration>::operator = (const QuoteType &v) {
	  value_.template emplace<QuoteType>(v);
	  return *this;
  }

  template <typename QuoteType, typename Duration>
  const types::Value& Quote<QuoteType, Duration>::value () const {
	  if (value_.Is<SingleQuote>()) {
		  return value_.As<SingleQuote>().value;
	  }
	  else if (value_.Is<types::Value>()) {
		  return value_.As<types::Value>();
	  }
	  else throw std::runtime_error(EXCEPTION_MSG("wrong Quote type; "));
  }


  template <typename QuoteType, typename Duration>
  types::Value& Quote<QuoteType, Duration>::value () {
	  if (value_.Is<SingleQuote>()) {
		  return const_cast<types::Value&>(value_.As<SingleQuote>().value);
	  }
	  else if (value_.Is<types::Value>()) {
		  return const_cast<types::Value&>(value_.As<types::Value>());
	  }
	  else throw std::runtime_error(EXCEPTION_MSG("wrong Quote type; "));
  }


  template <typename QuoteType, typename Duration>
  bool operator==(const Quote<QuoteType, Duration>& lhs, const Quote<QuoteType, Duration>& rhs){
	  if (lhs.timestamp != rhs.timestamp) return false;
	  return lhs.value() == rhs.value();
  }
  template <typename QuoteType, typename Duration>
  bool operator!=(const Quote<QuoteType, Duration>& lhs, const Quote<QuoteType, Duration>& rhs){
	  return not (rhs==lhs);
  }
  template <typename QuoteType, typename Duration>
  bool operator< (const Quote<QuoteType, Duration>& lhs, const Quote<QuoteType, Duration>& rhs) {
	  if (lhs.timestamp < rhs.timestamp) return true;
	  return lhs.value() < rhs.value();
  }
  template <typename QuoteType, typename Duration>
  bool operator> (const Quote<QuoteType, Duration>& lhs, const Quote<QuoteType, Duration>& rhs) {
	  return rhs < lhs;
  }
  template <typename QuoteType, typename Duration>
  bool operator<=(const Quote<QuoteType, Duration>& lhs, const Quote<QuoteType, Duration>& rhs) {
	  return not (rhs < lhs);
  }
  template <typename QuoteType, typename Duration>
  bool operator>=(const Quote<QuoteType, Duration>& lhs, const Quote<QuoteType, Duration>& rhs){
	  return not (lhs<rhs);
  }
  template <typename QuoteType, typename Duration>
  std::ostream& operator<<(std::ostream& os, const Quote<QuoteType, Duration>& quote) {
	  os << "timestamp: " << quote.timestamp << ", value: ";
	  quote_base::operator<<(os, quote.value_);
	  return os;
  }

}//!namespace

#endif //STRATEGY_TRADING_QUOTE_H
