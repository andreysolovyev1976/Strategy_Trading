//
// Created by Andrey Solovyev on 18/11/2021.
//

#include <gtest/gtest.h>
#include "data_processor.h"

#define DEBUG_OUTPUT_DEFINITION

TEST(TZKT_API, JsonResponseOk) {
	using namespace algo::tezos::tzkt_io;
	using namespace algo;

//	Ticker ticker ("KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT"); //test net Hangzhounet
	Ticker ticker ("KT1WBLrLE2vG8SedBqiSJFm4VVAZZBytJYHc"); //main net
	TradingContract trading_contract (ticker);
	DataProcessor data_processor;
	auto& response = data_processor.response;
	auto& request = data_processor.request;

	auto handle = getStorageQuery(ticker);
	response = request.
							  pathSetNew(std::move(handle))->
							  Implement(curl_client::Method::Get);

	std::visit(utils::overloaded{
			[]([[maybe_unused]] std::monostate arg) {/* do nothing */
#ifdef DEBUG_OUTPUT_DEFINITION
			  std::cerr << "no response\n";
#endif
			},
			[]([[maybe_unused]] const std::string& arg) {
			  std::stringstream ss(arg);
			  auto doc = Json::Load(ss);
#ifdef DEBUG_OUTPUT_DEFINITION
			  std::cerr << "String:\n";
			  Json::Print(doc, std::cerr);
#endif

			  auto& m = doc.GetRoot().AsDict();
			  auto found = m.find("storage");
			  ASSERT_TRUE(found != m.end());
			},
			[](Json::Document& arg) {
#ifdef DEBUG_OUTPUT_DEFINITION
			  std::cerr << "Json:\n";
			  Json::Print(arg, std::cerr);
#endif
			  auto& m = arg.GetRoot().AsDict();
			  auto found = m.find("storage");
			  ASSERT_TRUE(found != m.end());
			}
	}, response->body);
}

TEST(TZKT_API, JsonResponseNotOk) {
	using namespace algo::tezos::tzkt_io;
	using namespace algo;

	Ticker ticker ("UNKNOWN"); //test net Hangzhounet
	TradingContract trading_contract (ticker);
	DataProcessor data_processor;
	auto& response = data_processor.response;
	auto& request = data_processor.request;

	auto handle = getStorageQuery(ticker);
	response = request.
							  pathSetNew(std::move(handle))->
							  Implement(curl_client::Method::Get);

	/*
	Connection #0 to host api.tzkt.io left intact
	{"code":400,"errors":{"address":"Invalid KT1-address."}}
	 * */
	std::visit(utils::overloaded{
			[]([[maybe_unused]] std::monostate arg) {},
			[]([[maybe_unused]] const std::string& arg) {
			  std::stringstream ss(arg);
			  auto doc = Json::Load(ss);
			  auto& m = doc.GetRoot().AsDict();
			  auto found = m.find("storage");
			  ASSERT_FALSE(found != m.end());
			},
			[](Json::Document& arg) {
			  auto& m = arg.GetRoot().AsDict();
			  auto found = m.find("storage");
			  ASSERT_FALSE(found != m.end());
			}
	}, response->body);
}

TEST(TZKT_API, Factory_Structure_Contracts_Qty) {
	using namespace algo::tezos::tzkt_io;
	using namespace algo;


	std::vector<Ticker> tickers {
			//FA_1_2
			"KT1FWHLMk5tHbwuSsp31S4Jum4dTVmkXpfJw","KT1Lw8hCoaBrHeTeMXbqHPG4sS4K1xn7yKcD",
			//FA_2
			"KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ", "KT1SwH9P1Tx8a58Mm6qBExQFTcy2rwZyZiXS",
	};

	DataProcessor data_processor;
	auto& response = data_processor.response;
	auto& request = data_processor.request;

	for (const auto& ticker : tickers) {
		auto handle = getContractQuery(ticker);
		response = request.
								  pathSetNew(std::move(handle))->
								  Implement(curl_client::Method::Get);


		std::visit(utils::overloaded{
				[]([[maybe_unused]] std::monostate arg) {
				  throw RuntimeError("No response from a factory");
				},
				[]([[maybe_unused]] const std::string& arg) {
				  std::stringstream ss(arg);
				  auto doc = Json::Load(ss);
				  auto& m = doc.GetRoot().AsDict();
				  auto found = m.find("numContracts");
				  ASSERT_TRUE(found->second.AsInt() != 0);
//				  Print(doc, std::cerr);
				},
				[](Json::Document& arg) {
				  auto& m = arg.GetRoot().AsDict();
				  auto found = m.find("numContracts");
				  ASSERT_TRUE(found->second.AsInt() != 0);
//				  Print(arg, std::cerr);
				}
		}, response->body);
	}
}

#define DEBUG_OUTPUT(msg, print_headers) \
    if (debug_output) { \
    os << msg; \
    Print(response, os, print_headers); \
    os << '\n'; \
  }

Json::Document getContractData_(
		const types::String& factory_address,
		curl_client::Response& response, curl_client::Request& request,
		bool debug_output = false, std::ostream& os = std::cerr)
{

//useful thing
//	pathSetNew("https://api.tzkt.io/v1/contracts/KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ/interface")->
//		pathSetNew("https://api.tzkt.io/v1/contracts/KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ/bigmaps/token_to_exchange/keys/KT1Nbc9cmx19qFrYYFpkiDoojVYL8UZJYVcj")->

	types::String output = const_values::TZKT_BASE_ADDRESS;
	output += "contracts";
	output += '/';
	output += factory_address;
	output += '/';
	output += "bigmaps";
	output += '/';
	output += "token_to_exchange";
	output += '/';
	output += "keys";

	response = request.
							  pathSetNew(std::move(output))->
							  Implement(curl_client::Method::Get);

	DEBUG_OUTPUT("contracts: ", true)

	if (std::holds_alternative<Json::Document>(response.get()->body))
		return std::get<Json::Document>(response.get()->body);
	else return Json::Document(Json::Node(nullptr));
}


TEST(TZKT_API, Factory_Structure) {
	using namespace algo::tezos::tzkt_io;
	using namespace algo;


	std::vector<Ticker> factory_addresses {
			//FA_1_2
			"KT1FWHLMk5tHbwuSsp31S4Jum4dTVmkXpfJw","KT1Lw8hCoaBrHeTeMXbqHPG4sS4K1xn7yKcD",
			//FA_2
			"KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ", "KT1SwH9P1Tx8a58Mm6qBExQFTcy2rwZyZiXS",
	};

	Ticker contract ("KT1WBLrLE2vG8SedBqiSJFm4VVAZZBytJYHc");
	std::vector<Ticker> factory_found;

	DataProcessor data_processor;
	auto& response = data_processor.response;
	auto& request = data_processor.request;

	for (const auto& factory : factory_addresses) {
		const auto handle = getContractData_(factory, response, request);
#ifdef DEBUG_OUTPUT_DEFINITION
		Json::Print(handle, std::cerr);
#endif
		const auto tokens_to_exchange = handle.GetRoot().AsArray();
#ifdef DEBUG_OUTPUT_DEFINITION
		std::cerr << "Factory " << factory << ":\n";
#endif
		for (const auto& token : tokens_to_exchange) {
			const auto m = token.AsDict();
			auto key = m.find("key");
#ifdef DEBUG_OUTPUT_DEFINITION
			std::cerr << "\tkey: ";
#endif
			if (key->second.IsDict()) {
				auto token_address = key->second.AsDict().find("address");
#ifdef DEBUG_OUTPUT_DEFINITION
				std::cerr << token_address->second.AsString();
#endif
				if (token_address->second.AsString() == contract) {
					factory_found.push_back(factory);
					factory_found.back() += " found in keys";
				}
			}
			else if (key->second.IsString()) {
#ifdef DEBUG_OUTPUT_DEFINITION
				std::cerr << key->second.AsString();
#endif
				if (key->second.AsString() == contract) {
					factory_found.push_back(factory);
					factory_found.back() += " found in keys";
				}
			}
			auto value = m.find("value");
#ifdef DEBUG_OUTPUT_DEFINITION
			std::cerr << ", value: "<< value->second.AsString() << '\n';
#endif
			if (value->second.AsString() == contract) {
				factory_found.push_back(factory);
				factory_found.back() += " found in values";
#ifdef DEBUG_OUTPUT_DEFINITION
				std::cerr << "found here ^\n";
#endif

			}
		}
	}
	if (not factory_found.empty()) {
		for (const auto& found : factory_found) {
			std::cerr << found << '\n';
		}
	}
	else {
		std::cerr << "not found";
	}
}




TEST(TZKT_API, ContractData) {
	using namespace algo::tezos::tzkt_io;
	using namespace algo;

	std::vector<Ticker> dex_contracts {
			"KT1WBLrLE2vG8SedBqiSJFm4VVAZZBytJYHc", //fa_1_2
			"KT1U2hs5eNdeCpHouAvQXGMzGFGJowbhjqmo" //fa2
	};
	std::vector<Ticker> t_contracts {
			"KT1PWx2mnDueood7fEmfbBDKx1D9BAnnXitn", //fa_1_2
			"KT18fp5rcTW7mbWDmzFwjLDUhs5MeJmagDSZ" //fa2
	};
	DataProcessor data_processor;
	auto& response = data_processor.response;
	auto& request = data_processor.request;

	for (int i = 0; i != 2; ++i) {
		auto token = getContractData(dex_contracts[i], response, request);
		if (token.IsString()) {
			ASSERT_EQ(t_contracts[i], token.AsString());
		}
		else if (token.IsDict()) {
			const auto& m = token.AsDict();
			const auto& address = m.find("address");
			ASSERT_EQ(t_contracts[i], address->second.AsString());
		}
		else {
			throw RuntimeError(EXCEPTION_MSG("Unexpected token"));
		}
	}

}//!test


#undef DEBUG_OUTPUT_DEFINITION
#undef DEBUG_OUTPUT
