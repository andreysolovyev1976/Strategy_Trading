//
// Created by Andrey Solovyev on 06/12/2021.
//

#include "coinbase.h"

namespace algo {

  namespace coinbase {

#define DEBUG_OUTPUT(msg, print_headers) \
	if (debug_output) { \
	os << msg; \
	Print(response, os, print_headers); \
	os << '\n'; \
  }

	std::ostream& operator << (std::ostream& os, const CurrencyData &data_coinbase) {

		os << "details \n"
		   << '\t' << "crypto_address_link: " << data_coinbase.crypto_address_link << '\n'
		   << '\t' << "crypto_transaction_link: " << data_coinbase.crypto_transaction_link << '\n'
		   << '\t' << "display_name: " << data_coinbase.display_name << '\n'
		   << '\t' << "max_withdrawal_amount: " << data_coinbase.max_withdrawal_amount << '\n'
		   << '\t' << "min_withdrawal_amount: " << data_coinbase.min_withdrawal_amount << '\n'
		   << '\t' << "type: " << data_coinbase.type << '\n'
		   << "id: " << data_coinbase.id << '\n'
		   << "max_precision: " << data_coinbase.max_precision << '\n'
		   << "min_size: " << data_coinbase.min_size << '\n'
		   << "name: " << data_coinbase.name << '\n'
		   << "status: " << data_coinbase.status << '\n';

		return os;
	}

	Json::Document getListOfCurrencies (bool debug_output, std::ostream& os) {
		using namespace curl_client;

		Response response;
		Request request;

		//get currencies
		response = request.
								  pathSetNew("https://api.exchange.coinbase.com/currencies")->
								  Implement(Method::Get);

		DEBUG_OUTPUT("currencies: ", true)

		if (std::holds_alternative<Json::Document>(response.get()->body))
			return std::get<Json::Document>(response.get()->body);
		else return Json::Document(Json::Node(nullptr));
	}
	std::vector<CurrencyData> processListOfCurrencies (Json::Document document) {
		std::vector<CurrencyData> data_coinbase;
		if (not document.GetRoot().IsArray()) return data_coinbase;
		auto &currencies = const_cast<Json::Array&>(document.GetRoot().AsArray());
		data_coinbase.reserve(currencies.size());

		for (auto &currency : currencies) {
			data_coinbase.emplace_back(processSingleCurrency(std::move(currency)));
		}

		return data_coinbase;
	}
	std::vector<CurrencyData> processListOfCurrencies (Json::Node node) {
		std::vector<CurrencyData> data_coinbase;
		if (not node.IsArray()) return data_coinbase;
		auto &currencies = const_cast<Json::Array&>(node.AsArray());
		data_coinbase.reserve(currencies.size());

		for (auto &currency : currencies) {
			data_coinbase.emplace_back(processSingleCurrency(std::move(currency)));
		}

		return data_coinbase;
	}

	Json::Document getSingleCurrency (const types::String &ticker, bool debug_output, std::ostream& os) {
		using namespace curl_client;

		Response response;
		Request request;

		types::String path_new = "https://api.exchange.coinbase.com/currencies/";
		path_new += ticker;

		//get currency
		response = request.
								  pathSetNew(std::move(path_new))->
								  Implement(Method::Get);

		DEBUG_OUTPUT("currency ID: ", true)

		if (std::holds_alternative<Json::Document>(response.get()->body))
			return std::get<Json::Document>(response.get()->body);
		else return Json::Document(Json::Node(nullptr));
	}
	CurrencyData processSingleCurrency (Json::Document document) {
		CurrencyData data_coinbase;
		if (not document.GetRoot().IsDict()) return data_coinbase;
		auto &currency = const_cast<Json::Dict&>(document.GetRoot().AsDict());
		data_coinbase.empty = false;
		if (auto details = currency.find("details"); details != currency.end()) {
			if (details->second.IsDict()) {
				auto& details_dict = const_cast<Json::Dict&>(details->second.AsDict());

				if (auto found = details_dict.find("crypto_address_link");
						found!=details_dict.end() && found->second.IsString()) {
					data_coinbase.crypto_address_link = std::move(const_cast<std::string&>(found->second.AsString()));
				}
				if (auto found = details_dict.find("crypto_transaction_link");
						found!=details_dict.end() && found->second.IsString()) {
					data_coinbase.crypto_transaction_link = std::move(const_cast<std::string&>(found->second.AsString()));
				}
				if (auto found = details_dict.find("display_name");
						found!=details_dict.end() && found->second.IsString()) {
					data_coinbase.display_name = std::move(const_cast<std::string&>(found->second.AsString()));
				}
				if (auto found = details_dict.find("max_withdrawal_amount");
						found!=details_dict.end() && found->second.IsInt()) {
					data_coinbase.max_withdrawal_amount = found->second.AsInt();
				}
				if (auto found = details_dict.find("min_withdrawal_amount");
						found!=details_dict.end() && found->second.IsInt()) {
					data_coinbase.min_withdrawal_amount = found->second.AsInt();
				}
				if (auto found = details_dict.find("type");
						found!=details_dict.end() && found->second.IsString()) {
					data_coinbase.type = std::move(const_cast<std::string&>(found->second.AsString()));
				}
			}
		}
		if (auto found = currency.find("id");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.id = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = currency.find("max_precision");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.max_precision = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = currency.find("min_size");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.min_size = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = currency.find("name");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.name = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = currency.find("status");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.status = std::move(const_cast<std::string&>(found->second.AsString()));
		}

		return data_coinbase;
	}
	CurrencyData processSingleCurrency (Json::Node node) {
		CurrencyData data_coinbase;
		if (not node.IsDict()) return data_coinbase;
		auto &currency = const_cast<Json::Dict&>(node.AsDict());
		data_coinbase.empty = false;
		if (auto details = currency.find("details"); details != currency.end()) {
			if (details->second.IsDict()) {
				auto& details_dict = const_cast<Json::Dict&>(details->second.AsDict());

				if (auto found = details_dict.find("crypto_address_link");
						found!=details_dict.end() && found->second.IsString()) {
					data_coinbase.crypto_address_link = std::move(const_cast<std::string&>(found->second.AsString()));
				}
				if (auto found = details_dict.find("crypto_transaction_link");
						found!=details_dict.end() && found->second.IsString()) {
					data_coinbase.crypto_transaction_link = std::move(const_cast<std::string&>(found->second.AsString()));
				}
				if (auto found = details_dict.find("display_name");
						found!=details_dict.end() && found->second.IsString()) {
					data_coinbase.display_name = std::move(const_cast<std::string&>(found->second.AsString()));
				}
				if (auto found = details_dict.find("max_withdrawal_amount");
						found!=details_dict.end() && found->second.IsInt()) {
					data_coinbase.max_withdrawal_amount = found->second.AsInt();
				}
				if (auto found = details_dict.find("min_withdrawal_amount");
						found!=details_dict.end() && found->second.IsInt()) {
					data_coinbase.min_withdrawal_amount = found->second.AsInt();
				}
				if (auto found = details_dict.find("type");
						found!=details_dict.end() && found->second.IsString()) {
					data_coinbase.type = std::move(const_cast<std::string&>(found->second.AsString()));
				}
			}
		}
		if (auto found = currency.find("id");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.id = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = currency.find("max_precision");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.max_precision = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = currency.find("min_size");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.min_size = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = currency.find("name");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.name = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = currency.find("status");
				found != currency.end() && found->second.IsString()) {
			data_coinbase.status = std::move(const_cast<std::string&>(found->second.AsString()));
		}

		return data_coinbase;
	}


	Json::Document getSingleTradeData (const types::String &ticker1, const types::String &ticker2, bool debug_output, std::ostream& os) {
		using namespace curl_client;

		Response response;
		Request request;

		types::String path_new = "https://api.exchange.coinbase.com/products/";
		path_new += ticker1;
		path_new += '-';
		path_new += ticker2;
		path_new += "/ticker";

		//get ticker
		//Gets snapshot information about the last trade (tick), best bid/ask and 24h volume.
		response = request.
								  pathSetNew(std::move(path_new))->
								  Implement(Method::Get);
		/// 0.359 ms is a round trip

		DEBUG_OUTPUT("ticker product: ", true)

		if (std::holds_alternative<Json::Document>(response.get()->body))
			return std::get<Json::Document>(response.get()->body);
		else return Json::Document(Json::Node(nullptr));
	}
	TradeData processSingleTradeData (Json::Document document) {
		TradeData data_coinbase;
		if (not document.GetRoot().IsDict()) return data_coinbase;
		auto &trade = const_cast<Json::Dict&>(document.GetRoot().AsDict());
		data_coinbase.empty = false;
		if (auto found = trade.find("ask");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.ask = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("bid");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.bid = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("price");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.price = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("size");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.size = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("time");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.time = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("trade_id");
				found!=trade.end() && found->second.IsInt()) {
			data_coinbase.trade_id = found->second.AsInt();
		}
		if (auto found = trade.find("volume");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.volume = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		return data_coinbase;
	}
	TradeData processSingleTradeData (Json::Node node) {
		TradeData data_coinbase;
		if (not node.IsDict()) return data_coinbase;
		auto &trade = const_cast<Json::Dict&>(node.AsDict());
		data_coinbase.empty = false;
		if (auto found = trade.find("ask");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.ask = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("bid");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.bid = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("price");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.price = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("size");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.size = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("time");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.time = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		if (auto found = trade.find("trade_id");
				found!=trade.end() && found->second.IsInt()) {
			data_coinbase.trade_id = found->second.AsInt();
		}
		if (auto found = trade.find("volume");
				found != trade.end() && found->second.IsString()) {
			data_coinbase.volume = std::move(const_cast<std::string&>(found->second.AsString()));
		}
		return data_coinbase;
	}

#undef DEBUG_OUTPUT

  }//!namespace
}//!namespace
