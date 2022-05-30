//
// Created by Andrey Solovyev on 09/12/2021.
//

#include "tzkt_io.h"
#include "const_values.h"
#include "utils.h"

#include <optional>

namespace algo {
  namespace tezos {
	namespace tzkt_io {

	  using const_values::STRING_SIZE;

	  types::String getContractQuery (const types::String& contract_address) {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += const_values::TZKT_BASE_ADDRESS;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  return output;
	  }

	  types::String getStorageQuery (const types::String& contract_address) {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += const_values::TZKT_BASE_ADDRESS;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  output += '/';
		  output += "storage";
		  return output;
	  }
	  types::String getBigMapsQuery (const types::String& contract_address) {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += const_values::TZKT_BASE_ADDRESS;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  output += '/';
		  output += "bigmaps";
		  return output;

	  }
	  types::String getBigMapQuery (const types::String& contract_address, const types::String& bigmap_name) {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += const_values::TZKT_BASE_ADDRESS;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  output += '/';
		  output += "bigmaps";
		  output += '/';
		  output += bigmap_name;
		  return output;
	  }
	  types::String getBigMapKeysQuery (const types::String& contract_address, const types::String& bigmap_name) {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += const_values::TZKT_BASE_ADDRESS;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  output += '/';
		  output += "bigmaps";
		  output += '/';
		  output += bigmap_name;
		  output += '/';
		  output += "keys";
		  return output;
	  }
	  types::String getBigMapValueQuery (const types::String& contract_address, const types::String& bigmap_name, const types::String& key)  {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += const_values::TZKT_BASE_ADDRESS;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  output += '/';
		  output += "bigmaps";
		  output += '/';
		  output += bigmap_name;
		  output += '/';
		  output += "keys";
		  output += '/';
		  output += key;
		  return output;
	  }


#define DEBUG_OUTPUT(msg, print_headers) \
    if (debug_output) { \
    os << msg; \
    Print(response, os, print_headers); \
    os << '\n'; \
  }

	  Json::Node checkFactoryForToken (const Json::Document& doc, const types::String& ticker) {
		  if (not doc.GetRoot().IsArray()) {
			  throw RuntimeError(EXCEPTION_MSG("Response is not presented as Array"));
		  }
		  const auto& tokens = doc.GetRoot().AsArray();
		  for (const auto& token : tokens) {
			  if (not token.IsDict()) {
				  throw RuntimeError(EXCEPTION_MSG("Token is not presented as Map"));
			  }
			  const auto& m = token.AsDict();
			  const auto& value = m.find("value");
			  if (value->second == ticker) {
				  return m.find("key")->second;
			  }
		  }
		  return Json::Node{};
	  }

	  Json::Node getContractData(
			  const types::String& ticker,
			  curl_client::Response& response, curl_client::Request& request) {

		  Json::Node result;

		  for (const auto& factory : const_values::QUIPUSWAP_FACTORIES_ADDRESSES) {
			  auto handle = getBigMapKeysQuery(factory, "token_to_exchange");
			  response = request.
										pathSetNew(std::move(handle))->
										Implement(curl_client::Method::Get);

			  std::visit(utils::overloaded{
					  []([[maybe_unused]] std::monostate arg){
						throw RuntimeError(EXCEPTION_MSG("No response from a factory"));
					  },
					  [&ticker, &result]([[maybe_unused]] const std::string& arg) {
						std::stringstream ss(arg);
						auto doc = Json::Load(ss);
						result = checkFactoryForToken(doc, ticker);
					  },
					  [&ticker, &result](Json::Document& arg) {
						result = checkFactoryForToken(arg, ticker);
					  }
			  }, response->body);

			  if (not result.IsNull()) break;
		  }//!for
		  return result;
	  }//!fucn


	  Json::Document getContractData(
			  curl_client::Response& response, curl_client::Request& request,
			  bool debug_output, std::ostream& os)
	  {

//useful thing
//		  pathSetNew("https://api.tzkt.io/v1/contracts/KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ/interface")->

//		pathSetNew("https://api.tzkt.io/v1/contracts/KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ/bigmaps/token_to_exchange/keys/KT1Nbc9cmx19qFrYYFpkiDoojVYL8UZJYVcj")->

//		  token_to_exchange
		  //Gets contract data
		  response = request.
//									pathSetNew("https://api.tzkt.io/v1/contracts/KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ/views")->
									pathSetNew("see line 110 for details")->
									Implement(curl_client::Method::Get);

		  DEBUG_OUTPUT("contracts: ", true)

		  if (std::holds_alternative<Json::Document>(response.get()->body))
			  return std::get<Json::Document>(response.get()->body);
		  else return Json::Document(Json::Node(nullptr));
	  }


	  Json::Document getTransactionData(
			  curl_client::Response& response, curl_client::Request& request,
			  bool debug_output, std::ostream& os)
	  {

//		  надо проверить предыдущую реализацию бота
		  //Gets transactions
		  response = request.
//									pathSetNew("https://api.tzkt.io/v1/operations/transactions/?timestamp.ge=2021-12-08T23:00:00Z&limit=10000")->
//									pathSetNew("https://api.tzkt.io/v1/operations/transactions/oo2cCmVijU2bwiPTyLh8KU1wsWYJKuctRURBBzYnJDYEko2pZih")->
									Implement(curl_client::Method::Get);
		  /// 0.359 ms is a round trip

		  DEBUG_OUTPUT("txns: ", true)

		  if (std::holds_alternative<Json::Document>(response.get()->body))
			  return std::get<Json::Document>(response.get()->body);
		  else return Json::Document(Json::Node(nullptr));
	  }

#undef DEBUG_OUTPUT


	}//!namespace
  }//!namespace
}//!namespace
