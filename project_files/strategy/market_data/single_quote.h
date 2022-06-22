//
// Created by Andrey Solovyev on 19/06/2022.
//

#pragma once

#include "types_declarations.h"
#include "timestamp.h"


#ifndef STRATEGY_TRADING_SINGLE_QUOTE_H
#define STRATEGY_TRADING_SINGLE_QUOTE_H

namespace algo {
  namespace quote_base {
	template <typename Duration>
	struct SingleQuote {
		time_::Timestamp<Duration> timestamp;
		types::Value value{0}, volume{0};
		SingleQuote() = default;
	};
  }//!namespace
}//!namespace


#endif //STRATEGY_TRADING_SINGLE_QUOTE_H
