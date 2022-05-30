//
// Created by Andrey Solovyev on 18/11/2021.
//

#include <gtest/gtest.h>
#include "data_processor.h"

TEST(CoinBaseAPI, JsonResponseOk) {
	using namespace algo::data::coinbase;
	using namespace algo::quote_base;
	using namespace algo;
	using namespace time_;

	Ticker ticker ("XTZ-BTC");
	TradingContract trading_contract (ticker);
	DataProcessor data_processor;
	auto& response = data_processor.response;
	auto& request = data_processor.request;

	types::String path_new = const_values::COINBASE_API_PRODUCTS; //todo: unnecessary copy
	path_new += ticker;
	path_new += "/ticker";

	//get ticker
	//Gets snapshot information about the last trade (tick), best bid/ask and 24h volume.
	response = request.
							  pathSetNew(std::move(path_new))->
							  Implement(curl_client::Method::Get);

	ASSERT_TRUE(std::holds_alternative<Json::Document>(response.get()->body));
	const auto& doc = std::get<Json::Document>(response.get()->body).GetRoot();
	ASSERT_TRUE(doc.IsDict());
	const auto& trade = doc.AsDict();

	const std::vector<std::string> fields =
			{"ask", "bid", "price", "size", "time", "volume", "trade_id"};

	for (const auto& field : fields) {
		auto found = trade.find(field);
		ASSERT_TRUE(found!=trade.end()) << "Field not found in the response: " << field;
	}
}

TEST(CoinBaseAPI, JsonResponseNotOk) {
	using namespace algo::data::coinbase;
	using namespace algo::quote_base;
	using namespace algo;
	using namespace time_;

	Ticker ticker ("JAMES-BOND");
	TradingContract trading_contract (ticker);
	DataProcessor data_processor;
	auto& response = data_processor.response;
	auto& request = data_processor.request;

	types::String path_new = const_values::COINBASE_API_PRODUCTS; //todo: unnecessary copy
	path_new += ticker;
	path_new += "/ticker";

	//get ticker
	//Gets snapshot information about the last trade (tick), best bid/ask and 24h volume.
	response = request.
							  pathSetNew(std::move(path_new))->
							  Implement(curl_client::Method::Get);

	ASSERT_TRUE(std::holds_alternative<Json::Document>(response.get()->body));
	const auto& doc = std::get<Json::Document>(response.get()->body).GetRoot();
	ASSERT_TRUE(doc.IsDict());

	const auto& trade = doc.AsDict();
	auto found = trade.find("message");
	ASSERT_TRUE(found!=trade.end());
	ASSERT_TRUE(found->second.IsString());
	ASSERT_TRUE(found->second.AsString() == "NotFound");
}

TEST(CoinBaseAPI, CorrectTickerResponse) {
	using namespace algo::data::coinbase;
	using namespace algo::quote_base;
	using namespace algo;
	using namespace time_;

	TradingContract trading_contract ("XTZ-BTC");
	DataProcessor data_processor;

	using Duration = time_::Seconds;
	using QuoteType = types::Value;

	Quote<QuoteType, Duration> quote;
	auto tickers = utils::splitIntoWords(trading_contract.ticker, '-');
	ASSERT_EQ(tickers.size(), 2u);

	auto trade_data = getSingleTradeData(tickers[0], tickers[1], data_processor.response, data_processor.request);
	auto new_trade_data = processSingleTradeData(std::move(trade_data));
	ASSERT_FALSE(new_trade_data.empty);

	quote.timestamp = data_processor.getTimestamp<Duration>(std::move(new_trade_data.time));

	ASSERT_NO_THROW(
			quote.value_ = types::Value(new_trade_data.price, 6)
	);
}

TEST(CoinBaseAPI, IncorrectTickerResponse) {
	using namespace algo::data::coinbase;
	using namespace algo::quote_base;
	using namespace algo;
	using namespace time_;

	TradingContract trading_contract ("JAMES-BOND"); /// Invalid ticker
	DataProcessor data_processor;

	using Duration = time_::Seconds;
	using QuoteType = types::Value;

	Quote<QuoteType, Duration> quote;
	auto tickers = utils::splitIntoWords(trading_contract.ticker, '-');
	ASSERT_EQ(tickers.size(), 2u);

	auto trade_data = getSingleTradeData(tickers[0], tickers[1], data_processor.response, data_processor.request);
	auto new_trade_data = processSingleTradeData(std::move(trade_data));
	ASSERT_EQ(new_trade_data.empty, true);
	ASSERT_ANY_THROW(quote.value_ = types::Value(new_trade_data.price, 6));
}