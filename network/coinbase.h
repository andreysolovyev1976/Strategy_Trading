//
// Created by Andrey Solovyev on 06/12/2021.
//

#pragma once

#include "types_declarations.h"
#include "curl_client/curl_client.h"

#ifndef STRATEGY_TRADING_NETWORK_H
#define STRATEGY_TRADING_NETWORK_H

namespace algo {
  namespace coinbase {

    static const auto input_fmt = []() -> types::String {return "%Y-%m-%dT%H:%M:%S";}();

	struct CurrencyData {
		bool empty = true;
		types::String
				crypto_address_link,
				crypto_transaction_link,
				display_name,
				type,
				id,
				max_precision,
				min_size,
				name,
				status;
		types::Int
				max_withdrawal_amount,
				min_withdrawal_amount;

	};

	std::ostream& operator << (std::ostream& os, const CurrencyData &data_coinbase);

	template <typename ...Fields>
	void Print(const CurrencyData &data_coinbase, std::ostream& os, Fields ...fields) {
		((os << data_coinbase.*fields << ' '), ...);
	}
	template <typename ...Fields>
	void Print(const std::vector<CurrencyData> &data_coinbase, std::ostream& os, Fields ...fields) {
		for (const auto& currency : data_coinbase) {
			Print(currency, os, fields ...);
			os << '\n';
		}
	}

	struct TradeData {
		bool empty = true;
		types::String
				ask,
				bid,
				price,
				size,
				time,
				volume;
		types::Int trade_id;
	};

	Json::Document getListOfCurrencies (bool debug_output = false, std::ostream& os = std::cerr);
	std::vector<CurrencyData> processListOfCurrencies (Json::Document document);
	std::vector<CurrencyData> processListOfCurrencies (Json::Node node);

	Json::Document getSingleCurrency (const types::String &ticker, bool debug_output = false, std::ostream& os = std::cerr);
	CurrencyData processSingleCurrency (Json::Document document);
	CurrencyData processSingleCurrency (Json::Node node);

	Json::Document getSingleTradeData (const types::String &ticker1, const types::String &ticker2, bool debug_output = false, std::ostream& os = std::cerr);
	TradeData processSingleTradeData (Json::Document document);
	TradeData processSingleTradeData (Json::Node node);

  }//!namespace
}//!namespace


#endif //STRATEGY_TRADING_NETWORK_H
