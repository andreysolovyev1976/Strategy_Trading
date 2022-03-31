//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "trading_contract.h"

namespace algo {

  char TradingContract::checkDex () const {
  	if (auto space =
  			std::find_if(std::begin(ticker), std::end(ticker), [](char c){return c == '-';});
  	space != std::end(ticker)) {
  		return 'C';
  	} else {
  		return 'Q';
  	}
  }

}