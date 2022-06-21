//
// Created by Andrey Solovyev on 06/11/2021.
//

#pragma once

#include "object_type.h"
#include "trading_contract.h"
#include "timestamp.h"
#include "curl_client/utils.h"

#include <iomanip>

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
//	  template <typename Timetick = time_::Seconds>
	  struct OHLCV {
		  types::Value
				  open, high, low, close, volume;
//		  [[maybe_unused]] Timetick timetick;
		  std::vector<types::Value> data;
	  };
	}//!namespace

//	template <typename Timetick = time_::Seconds>
	using QuoteTypeBase =
	std::variant<
			std::monostate,
			types::Value,
			quote_type::SingleQuote,
			quote_type::BidAsk,
			quote_type::OHLCV
//			quote_type::OHLCV<Timetick>
	>;

//	template <typename Timetick = time_::Seconds>
//	std::ostream& operator << (std::ostream& os, const QuoteTypeBase<Timetick> &data) {

	inline //just to avoid another .cpp file, leaving this for now, until there is no solution for OHLCV
	std::ostream& operator << (std::ostream& os, const QuoteTypeBase &data) {
		using namespace quote_type;
		std::visit(utils::overloaded{
				[]([[maybe_unused]] std::monostate arg) {/* do nothing  */ },
				[&os](types::Value arg) {
				  os << std::setprecision(arg.precision) << std::fixed << arg;
				},
				[&os](const SingleQuote &arg) {
				  os << arg.value;
				},
				[&os](const BidAsk &arg) {
				  os
						  << "bid: " 		<< std::setprecision(arg.bid.precision) 	<< std::fixed << arg.bid
						  << "; ask: " 	  	<< std::setprecision(arg.ask.precision) 	<< std::fixed << arg.ask
						  << "; middle: "	<< std::setprecision(arg.middle.precision) 	<< std::fixed << arg.middle
						  << "; price: "	<< std::setprecision(arg.price.precision) 	<< std::fixed << arg.price;
				},
				[&os](const OHLCV &arg) {
//				[&os](const OHLCV<Timetick> &arg) {
				  os
						  << "open: " 		<< std::setprecision(arg.open.precision) 	<< std::fixed << arg.open
						  << "; high: " 	<< std::setprecision(arg.high.precision) 	<< std::fixed << arg.high
						  << "; low: "		<< std::setprecision(arg.low.precision) 	<< std::fixed << arg.low
						  << "; close: "	<< std::setprecision(arg.close.precision) 	<< std::fixed << arg.close
						  << "; volume: "	<< std::setprecision(arg.volume.precision) 	<< std::fixed << arg.volume;
				},
		}, data);
		return os;
	}//!operator

//	template <typename Timetick = time_::Seconds>
//	struct QuoteValue final : public types::ObjectType<QuoteTypeBase<Timetick>> {};

	struct QuoteValue final : public types::ObjectType<QuoteTypeBase> {};

  }//!namespace

  using namespace quote_base::quote_type;

  //todo: finalize OHLCV, add TimeSeries as aggregating mechanism

  template <typename QuoteType = types::Value, typename Duration = time_::Milliseconds>
  struct Quote final {
	  Quote() = default;

	  Quote(QuoteType v);
	  Quote& operator = (const QuoteType &v);

	  const types::Value& value () const;
	  types::Value& value ();
//	  const QuoteType& value () const;
//	  QuoteType& value ();

	  time_::Timestamp<Duration> timestamp;
	  //todo: refactor member - remove lower dash
//	  quote_base::QuoteValue value_;
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
//  const QuoteType& Quote<QuoteType, Duration>::value () const {
	const types::Value& Quote<QuoteType, Duration>::value () const {
	  if (value_.Is<types::Value>()) {
		  return value_.As<types::Value>();
	  }
	  else if (value_.Is<SingleQuote>()) {
		  return value_.As<SingleQuote>().value;
	  }
	  /*
	  else if (value_.Is<BidAsk>()) {
		  return value_.As<BidAsk>();
	  }
	  else if (value_.Is<OHLCV>()) {
		  return value_.As<OHLCV>();
	  }
	   */
	  else throw std::runtime_error(EXCEPTION_MSG("Value is not yet initialized; "));
  }


  //todo: this is strange implementation, should be able to assign a new value, why throw?
  template <typename QuoteType, typename Duration>
//  QuoteType& Quote<QuoteType, Duration>::value () {
	types::Value& Quote<QuoteType, Duration>::value () {
	  if (value_.Is<types::Value>()) {
		  return const_cast<types::Value&>(value_.As<types::Value>());
	  }
	  else if (value_.Is<SingleQuote>()) {
		  return const_cast<types::Value&>(value_.As<SingleQuote>().value);
	  }
/*
	  else if (value_.Is<BidAsk>()) {
		  return const_cast<BidAsk&>(value_.As<BidAsk>());
	  }
	  else if (value_.Is<OHLCV>()) {
		  return const_cast<OHLCV&>(value_.As<OHLCV>());
	  }
	  */
	  else throw std::runtime_error(EXCEPTION_MSG("Value is not yet initialized; "));
  }

  inline //just to avoid another .cpp file
  bool operator==(const quote_base::quote_type::BidAsk& lhs, const quote_base::quote_type::BidAsk& rhs){
	  return
			  lhs.ask == rhs.ask &&
					  lhs.bid == rhs.bid &&
					  lhs.middle == rhs.middle &&
					  lhs.price == rhs.price;
  }

  inline //just to avoid another .cpp file
  bool operator==(const quote_base::quote_type::OHLCV& lhs, const quote_base::quote_type::OHLCV& rhs){
	  return
			  lhs.open == rhs.open &&
					  lhs.high == rhs.high &&
					  lhs.low == rhs.low &&
					  lhs.close == rhs.close &&
					  lhs.volume == rhs.volume;
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
	  os << "timestamp: " << quote.timestamp << ", quote: ";
	  quote_base::operator<<(os, quote.value_);
	  return os;
  }

}//!namespace

#endif //STRATEGY_TRADING_QUOTE_H
