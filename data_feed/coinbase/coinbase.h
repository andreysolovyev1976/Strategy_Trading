//
// Created by Andrey Solovyev on 06/12/2021.
//

#pragma once

#include "types_declarations.h"
#include "curl_client.h"
#include "coinbase_data_structures.h"

#ifndef STRATEGY_TRADING_NETWORK_H
#define STRATEGY_TRADING_NETWORK_H

namespace algo {
  namespace data {
	namespace coinbase {


	  std::ostream& operator<<(std::ostream& os, const CurrencyData& data_coinbase);

	  template<typename ...Fields>
	  void Print(const CurrencyData& data_coinbase, std::ostream& os, Fields ...fields)
	  {
		  ((os << data_coinbase.*fields << ' '), ...);
	  }
	  template<typename ...Fields>
	  void Print(const std::vector<CurrencyData>& data_coinbase, std::ostream& os, Fields ...fields)
	  {
		  for (const auto& currency : data_coinbase) {
			  Print(currency, os, fields ...);
			  os << '\n';
		  }
	  }

	  Json::Document getListOfCurrencies(bool debug_output = false, std::ostream& os = std::cerr);
	  std::vector<CurrencyData> processListOfCurrencies(Json::Document document);
	  std::vector<CurrencyData> processListOfCurrencies(Json::Node node);

	  Json::Document getSingleCurrency(const types::String& ticker, bool debug_output = false,
			  std::ostream& os = std::cerr);
	  CurrencyData processSingleCurrency(Json::Document document);
	  CurrencyData processSingleCurrency(Json::Node node);

	  Json::Document getSingleTradeData(const types::String& ticker1, const types::String& ticker2,
			  bool debug_output = false, std::ostream& os = std::cerr);
	  TradeData processSingleTradeData(Json::Document document);
	  TradeData processSingleTradeData(Json::Node node);

	}//!namespace
  }//!namespace
}//!namespace


#endif //STRATEGY_TRADING_NETWORK_H
