//
// Created by Andrey Solovyev on 18/11/2021.
//

#include <gtest/gtest.h>
#include "data_processor.h"

TEST(TZKT_API, JsonResponseOk) {
	using namespace algo::tezos::tzkt_io;
	using namespace algo;

	Ticker ticker ("KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT"); //test net Hangzhounet
	TradingContract trading_contract (ticker);
	DataProcessor data_processor;
	auto& response = data_processor.response;
	auto& request = data_processor.request;

	auto handle = getStorageQuery(ticker);
	response = request.
							  pathSetNew(std::move(handle))->
							  Implement(curl_client::Method::Get);

	std::visit(utils::overloaded{
			[]([[maybe_unused]] std::monostate arg) {},
			[]([[maybe_unused]] const std::string& arg) {
			  std::stringstream ss(arg);
			  auto doc = Json::Load(ss);
			  auto& m = doc.GetRoot().AsDict();
			  auto found = m.find("storage");
			  ASSERT_TRUE(found != m.end());
			},
			[](Json::Document& arg) {
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

