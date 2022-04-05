//
// Created by Andrey Solovyev on 09/12/2021.
//

#include "tzkt_io.h"
#include "const_values.h"


namespace algo {
  namespace tezos {
	namespace tzkt_io {

	  using const_values::STRING_SIZE;

	  types::String QueryFactory::getContractQuery (const types::String& contract_address) const {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += base_address;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  return output;
	  }
	  types::String QueryFactory::getStorageQuery (const types::String& contract_address) const {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += base_address;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  output += '/';
		  output += "storage";
		  return output;
	  }
	  types::String QueryFactory::getBigMapsQuery (const types::String& contract_address) const {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += base_address;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  output += '/';
		  output += "bigmaps";
		  return output;

	  }
	  types::String QueryFactory::getBigMapQuery (const types::String& contract_address, const types::String& bigmap_name) const {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += base_address;
		  output += "contracts";
		  output += '/';
		  output += contract_address;
		  output += '/';
		  output += "bigmaps";
		  output += '/';
		  output += bigmap_name;
		  return output;
	  }
	  types::String QueryFactory::getBigMapKeysQuery (const types::String& contract_address, const types::String& bigmap_name) const {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += base_address;
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
	  types::String QueryFactory::getBigMapValueQuery (const types::String& contract_address, const types::String& bigmap_name, const types::String& key) const {
		  types::String output;
		  output.reserve(STRING_SIZE);
		  output += base_address;
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


	  Json::Document getContractData(
			  curl_client::Response& response, curl_client::Request& request,
			  bool debug_output, std::ostream& os)
	  {

//useful thing
//		  pathSetNew("https://api.tzkt.io/v1/contracts/KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ/interface")->

//		  token_to_exchange
		  //Gets contract data
		  response = request.
//									pathSetNew("https://api.tzkt.io/v1/contracts/KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ/views")->
									pathSetNew("https://api.tzkt.io/v1/contracts/KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ/bigmaps/token_to_exchange/keys/KT1Nbc9cmx19qFrYYFpkiDoojVYL8UZJYVcj")->
									Implement(curl_client::Method::Get);
		  /// 0.359 ms is a round trip

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


#if 0
	  TradeData processSingleTransactionData(Json::Document document)
	  {
		  TradeData data_tzkt_io;
		  if (not document.GetRoot().IsDict()) return data_tzkt_io;
		  auto& trade = const_cast<Json::Dict&>(document.GetRoot().AsDict());
		  data_tzkt_io.empty = false;
		  if (auto found = trade.find("ask");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.ask = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("bid");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.bid = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("price");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.price = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("size");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.size = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("time");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.time = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("trade_id");
				  found!=trade.end() && found->second.IsInt()) {
			  data_tzkt_io.trade_id = found->second.AsInt();
		  }
		  if (auto found = trade.find("volume");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.volume = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  return data_tzkt_io;
	  }
#endif
#if 0
	  TradeData processSingleTransactionData(Json::Node node)
	  {
		  TradeData data_tzkt_io;
		  if (not node.IsDict()) return data_tzkt_io;
		  auto& trade = const_cast<Json::Dict&>(node.AsDict());
		  data_tzkt_io.empty = false;
		  if (auto found = trade.find("ask");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.ask = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("bid");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.bid = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("price");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.price = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("size");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.size = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("time");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.time = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("trade_id");
				  found!=trade.end() && found->second.IsInt()) {
			  data_tzkt_io.trade_id = found->second.AsInt();
		  }
		  if (auto found = trade.find("volume");
				  found!=trade.end() && found->second.IsString()) {
			  data_tzkt_io.volume = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  return data_tzkt_io;
	  }
#endif

#undef DEBUG_OUTPUT


	}//!namespace
  }//!namespace
}//!namespace
