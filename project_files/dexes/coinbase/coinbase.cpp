//
// Created by Andrey Solovyev on 06/12/2021.
//

#include "coinbase.h"

namespace algo {
  namespace data {
	namespace coinbase {

#define DEBUG_OUTPUT(msg, print_headers) \
    if (debug_output) { \
    os << msg; \
    Print(response, os, print_headers); \
    os << '\n'; \
  }

	  std::ostream& operator<<(std::ostream& os, const CurrencyData& data_coinbase)
	  {

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

	  Json::Document processAPIResponse (curl_client::Response&& response) {
		  //todo: add to logging errors received
		  if (std::holds_alternative<Json::Document>(response.get()->body)) {
			  return std::move(std::get<Json::Document>(response.get()->body));
		  }
		  else if (std::holds_alternative<std::string>(response.get()->body)) {
			  std::stringstream ss(std::get<std::string>(response.get()->body));
			  auto doc = Json::Load(ss);
			  return doc;
		  }
	  return Json::Document(Json::Node(nullptr));
	}//!func

	Json::Document getListOfCurrencies(
			curl_client::Response& response, curl_client::Request& request,
			bool debug_output, std::ostream& os)
	{

		types::String path_new = const_values::COINBASE_API_CURRENCIES; //todo: unnecessary copy
		//get currencies
		response = request.
								  pathSetNew(std::move(path_new))->
								  Implement(curl_client::Method::Get);

		DEBUG_OUTPUT("currencies: ", true)
		return processAPIResponse(std::move(response));
	}

	Json::Document getSingleCurrency(const types::String& ticker,
			curl_client::Response& response, curl_client::Request& request,
			bool debug_output, std::ostream& os)
	{
		types::String path_new = const_values::COINBASE_API_CURRENCIES; //todo: unnecessary copy
		path_new += ticker;

		//get currency
		response = request.
								  pathSetNew(std::move(path_new))->
								  Implement(curl_client::Method::Get);

		DEBUG_OUTPUT("currency ID: ", true)
		return processAPIResponse(std::move(response));
	}

	Json::Document getSingleTradeData(
			const types::String& ticker1, const types::String& ticker2,
			curl_client::Response& response, curl_client::Request& request,
			bool debug_output, std::ostream& os)
	{
		types::String path_new = const_values::COINBASE_API_PRODUCTS; //todo: unnecessary copy
		path_new += ticker1;
		path_new += '-';
		path_new += ticker2;
		path_new += "/ticker";

		//get ticker
		//Gets snapshot information about the last trade (tick), best bid/ask and 24h volume.
		response = request.
								  pathSetNew(std::move(path_new))->
								  Implement(curl_client::Method::Get);
		/// 0.359 ms is a round trip

		DEBUG_OUTPUT("ticker product: ", true)
		return processAPIResponse(std::move(response));
	}

#undef DEBUG_OUTPUT

  }//!namespace
}//!namespace
}//!namespace
