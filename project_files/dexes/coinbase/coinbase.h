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


	  ///Declarations
	  std::ostream& operator<<(std::ostream& os, const CurrencyData& data_coinbase);

	  template<typename ...Fields>
	  void Print(const CurrencyData& data_coinbase, std::ostream& os, Fields ...fields);
	  template<typename ...Fields>
	  void Print(const std::vector<CurrencyData>& data_coinbase, std::ostream& os, Fields ...fields);

	  Json::Document getListOfCurrencies(
			  curl_client::Response& response, curl_client::Request& request,
			  bool debug_output = false, std::ostream& os = std::cerr);

	  template <typename Input>
	  std::vector<CurrencyData> processListOfCurrencies(Input input);

	  Json::Document getSingleCurrency(const types::String& ticker,
			  curl_client::Response& response, curl_client::Request& request,
			  bool debug_output = false, std::ostream& os = std::cerr);

	  template <typename Input>
	  CurrencyData processSingleCurrency(Input input);

	  Json::Document getSingleTradeData(
			  const types::String& ticker1, const types::String& ticker2,
			  curl_client::Response& response, curl_client::Request& request,
			  bool debug_output = false, std::ostream& os = std::cerr);

	  template <typename Input>
	  TradeData processSingleTradeData(Input input);



	  ///Definitions

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


	  template <typename Input>
	  std::vector<CurrencyData> processListOfCurrencies(Input input)
	  {
		  std::vector<CurrencyData> data_coinbase;

		  const Json::Node *node;
		  if constexpr (std::is_same_v<std::decay_t<Input>, Json::Document>) node = &input.GetRoot();
		  else if constexpr (std::is_same_v<std::decay_t<Input>, Json::Node>) node = &input;
		  else return data_coinbase;

		  if (not node->IsArray()) return data_coinbase;
		  auto& currencies = const_cast<Json::Array&>(node->AsArray());
		  data_coinbase.reserve(currencies.size());

		  for (auto& currency : currencies) {
			  data_coinbase.emplace_back(processSingleCurrency(std::move(currency)));
		  }

		  return data_coinbase;
	  }


	  template <typename Input>
	  CurrencyData processSingleCurrency(Input input)
	  {
		  CurrencyData data_coinbase;

		  const Json::Node *node;
		  if constexpr (std::is_same_v<std::decay_t<Input>, Json::Document>) node = &input.GetRoot();
		  else if constexpr (std::is_same_v<std::decay_t<Input>, Json::Node>) node = &input;
		  else return data_coinbase;

		  if (not node->IsDict()) return data_coinbase;
		  auto& currency = const_cast<Json::Dict&>(node->AsDict());
		  data_coinbase.empty = false;
		  if (auto details = currency.find("details"); details!=currency.end()) {
			  if (details->second.IsDict()) {
				  auto& details_dict = const_cast<Json::Dict&>(details->second.AsDict());

				  if (auto found = details_dict.find("crypto_address_link");
						  found!=details_dict.end() && found->second.IsString()) {
					  data_coinbase.crypto_address_link = std::move(const_cast<std::string&>(found->second.AsString()));
				  }
				  if (auto found = details_dict.find("crypto_transaction_link");
						  found!=details_dict.end() && found->second.IsString()) {
					  data_coinbase.crypto_transaction_link = std::move(
							  const_cast<std::string&>(found->second.AsString()));
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
				  found!=currency.end() && found->second.IsString()) {
			  data_coinbase.id = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = currency.find("max_precision");
				  found!=currency.end() && found->second.IsString()) {
			  data_coinbase.max_precision = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = currency.find("min_size");
				  found!=currency.end() && found->second.IsString()) {
			  data_coinbase.min_size = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = currency.find("name");
				  found!=currency.end() && found->second.IsString()) {
			  data_coinbase.name = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = currency.find("status");
				  found!=currency.end() && found->second.IsString()) {
			  data_coinbase.status = std::move(const_cast<std::string&>(found->second.AsString()));
		  }

		  return data_coinbase;
	  }


	  template <typename Input>
	  TradeData processSingleTradeData(Input input)
	  {
		  TradeData data_coinbase;

		  const Json::Node *node;
		  if constexpr (std::is_same_v<std::decay_t<Input>, Json::Document>) node = &input.GetRoot();
		  else if constexpr (std::is_same_v<std::decay_t<Input>, Json::Node>) node = &input;
		  else return data_coinbase;

		  if (not node->IsDict()) return data_coinbase;
		  auto& trade = const_cast<Json::Dict&>(node->AsDict());
		  data_coinbase.empty = false;
		  if (auto found = trade.find("ask");
				  found!=trade.end() && found->second.IsString()) {
			  data_coinbase.ask = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("bid");
				  found!=trade.end() && found->second.IsString()) {
			  data_coinbase.bid = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("price");
				  found!=trade.end() && found->second.IsString()) {
			  data_coinbase.price = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("size");
				  found!=trade.end() && found->second.IsString()) {
			  data_coinbase.size = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("time");
				  found!=trade.end() && found->second.IsString()) {
			  data_coinbase.time = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  if (auto found = trade.find("trade_id");
				  found!=trade.end() && found->second.IsInt()) {
			  data_coinbase.trade_id = found->second.AsInt();
		  }
		  if (auto found = trade.find("volume");
				  found!=trade.end() && found->second.IsString()) {
			  data_coinbase.volume = std::move(const_cast<std::string&>(found->second.AsString()));
		  }
		  return data_coinbase;
	  }


	}//!namespace
  }//!namespace
}//!namespace


#endif //STRATEGY_TRADING_NETWORK_H
