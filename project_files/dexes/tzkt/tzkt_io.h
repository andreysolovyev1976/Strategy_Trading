//
// Created by Andrey Solovyev on 09/12/2021.
//

#pragma once

#include "types_declarations.h"
#include "json.h"
#include "curl_client.h"
#include "tezos_data_structures.h"

#ifndef STRATEGY_TRADING_TZKT_IO_H
#define STRATEGY_TRADING_TZKT_IO_H

namespace algo {
  namespace tezos {
	namespace tzkt_io {

	  types::String getContractQuery (const types::String& contract_address);
	  types::String getStorageQuery (const types::String& contract_address);
	  types::String getBigMapsQuery (const types::String& contract_address);
	  types::String getBigMapQuery (const types::String& contract_address, const types::String& bigmap_name);
	  types::String getBigMapKeysQuery (const types::String& contract_address, const types::String& bigmap_name);
	  types::String getBigMapValueQuery (const types::String& contract_address, const types::String& bigmap_name, const types::String& key);


	  struct TradeData {
		  struct Details {
			  types::String address, alias;
		  };
		  bool empty = true;
		  types::String type, timestamp, status;
		  types::UInt id, level, amount;
		  Details initiator, sender, target;
	  };

	  Json::Document getContractData(
			  curl_client::Response& response, curl_client::Request& request,
			  bool debug_output, std::ostream& os);


	  Json::Document getTransactionData(
			  curl_client::Response& response, curl_client::Request& request,
			  bool debug_output = false, std::ostream& os = std::cerr);
	  TradeData processSingleTransactionData(Json::Document document);
	  TradeData processSingleTransactionData(Json::Node node);

	}//!namespace
  }//!namespace
}//!namespace

#endif //STRATEGY_TRADING_TZKT_IO_H
