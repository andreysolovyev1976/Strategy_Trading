//
// Created by Andrey Solovyev on 06/11/2021.
//

#include "quote.h"
#include "curl_client/utils.h"

namespace algo {
  namespace quote_base {

	std::ostream& operator << (std::ostream& os, const QuoteTypeBase &data) {
		std::visit(utils::overloaded{
				[]([[maybe_unused]] std::monostate arg) {/* do nothing  */ },
				[&os](types::Value arg) {
				  	os << arg;
					},
				[&os](const SingleQuote &arg) {
				  os << arg.value;
				},
				[&os](const BidAsk &arg) {
				  os
						  << "bid: " << arg.bid
						  << "; ask: " << arg.ask
						  << "; middle: " << arg.middle
						  << "; price: " << arg.price;
				},
				[&os](const OHLCV &arg) {
				  os
						  << "open: " << arg.open
						  << "; high: " << arg.high
						  << "; low: " << arg.low
						  << "; close: " << arg.close
						  << "; volume: " << arg.volume;
				},
		}, data);
		return os;
	}
  }//!namespace
}//!namespace


