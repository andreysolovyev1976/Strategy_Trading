//
// Created by Andrey Solovyev on 06/11/2021.
//

#include "quote.h"
#include "curl_client/utils.h"

#include <iomanip>

namespace algo {
  namespace quote_base {

	std::ostream& operator << (std::ostream& os, const QuoteTypeBase &data) {
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
				  os
						  << "open: " 		<< std::setprecision(arg.open.precision) 	<< std::fixed << arg.open
						  << "; high: " 	<< std::setprecision(arg.high.precision) 	<< std::fixed << arg.high
						  << "; low: "		<< std::setprecision(arg.low.precision) 	<< std::fixed << arg.low
						  << "; close: "	<< std::setprecision(arg.close.precision) 	<< std::fixed << arg.close
						  << "; volume: "	<< std::setprecision(arg.volume.precision) 	<< std::fixed << arg.volume;
				},
		}, data);
		return os;
	}
  }//!namespace
}//!namespace


