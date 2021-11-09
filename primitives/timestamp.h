//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include <ctime>
#include <iosfwd>

#ifndef STRATEGY_TRADING_TIMESTAMP_H
#define STRATEGY_TRADING_TIMESTAMP_H

namespace algo {
  namespace timestamp {

  	types::String makeDateTimeString(time_t t, bool isUTC);

  	inline types::String makeLocalDateTimeString(time_t t) {
  		return makeDateTimeString(t, false);
  	}

  	inline types::String makeUTCDateTimeString(time_t t) {
  		return makeDateTimeString(t, true);
  	}

  	//todo: EXCHANGE IT WITH C++20 FEATURE
  	struct Timestamp {
  		// add operator < and operator ==

  		Timestamp() = default;
  		Timestamp(types::String value);
		types::String value;
  	};

  	bool operator <(const Timestamp& lhs, const Timestamp& rhs);
  	bool operator >(const Timestamp& lhs, const Timestamp& rhs);
  	bool operator<=(const Timestamp& lhs, const Timestamp& rhs);
  	bool operator>=(const Timestamp& lhs, const Timestamp& rhs);
  	bool operator==(const Timestamp& lhs, const Timestamp& rhs);
  	bool operator!=(const Timestamp& lhs, const Timestamp& rhs);
  	std::ostream& operator<<(std::ostream& os, const Timestamp& timestamp);


  }//!namespace
}//!namespace




#endif //STRATEGY_TRADING_TIMESTAMP_H
