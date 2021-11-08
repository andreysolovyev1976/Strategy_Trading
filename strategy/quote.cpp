//
// Created by Andrey Solovyev on 06/11/2021.
//

#include "quote.h"

namespace algo {

  using namespace trade_base;

  Quote::Quote(timestamp::Timestamp timestamp, types::Value value)
  :timestamp(std::move(timestamp)), value(std::move(value)) { }

  bool operator==(const Quote& lhs, const Quote& rhs){
  	if (lhs.timestamp != rhs.timestamp) return false;
  	return lhs.value == rhs.value;
  }
  bool operator!=(const Quote& lhs, const Quote& rhs){
  	return not (rhs==lhs);
  }
  bool operator< (const Quote& lhs, const Quote& rhs) {
  	if (lhs.timestamp < rhs.timestamp) return true;
  	return lhs.value<rhs.value;
  }
  bool operator> (const Quote& lhs, const Quote& rhs) {
  	return rhs < lhs;
  }
  bool operator<=(const Quote& lhs, const Quote& rhs) {
  	return not (rhs < lhs);
  }
  bool operator>=(const Quote& lhs, const Quote& rhs){
  	return not (lhs<rhs);
  }
  std::ostream& operator<<(std::ostream& os, const Quote& quote) {
  	os << "timestamp: " << quote.timestamp << ", quote: " << quote.value;
  	return os;
  }

}//!namespace


