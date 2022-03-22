#include "coinbase.h"
#include "tzkt_io.h"
#include "quipuswap.h"
#include "strategy.h"
#include "randomer.h"

#include "tg_bot_ui.h"

#include <iostream>
#include <iomanip>
#include <array>

#include <chrono>
#include <thread>

#include "bot_config.h"


#include "dex_transaction.h"


using namespace algo;
using namespace std;
using namespace std::literals;
using namespace std::chrono_literals;
using namespace boost;
using namespace algo::data::coinbase;
using namespace algo::tezos::tzkt_io;
using namespace algo::tezos::quipuswap;
using namespace time_;

static auto const fastIO = []() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  return 0;
}();

[[maybe_unused]]
algo::Quote<types::Value, time_::Milliseconds>
        modifier (const algo::Quote<types::Value, time_::Milliseconds>& quote){
	Quote<types::Value, time_::Milliseconds> modified = quote;
	modified.value() = quote.value() - types::Value(10);
	return modified;
}

int main() {

	//todo: Check The Rules - add tickers
	//todo: check Indicator and Signal updates
	//todo: Add ThreadSafe maps
	//todo: Add Map size limit
	//todo: Add Value ctor from Json
	//todo: Add Logging at large
	//todo: Unclear what happens with a Token price - need to check that


	//todo: add a case when there is a new data for some of the labels in strategy - probably check that when ruleSignaling

#if 0
	auto trade1 = processSingleTradeData(getSingleTradeData("ETH", "USD"));
	std::stringstream ss (trade1.time);

	std::cerr << trade1.time << '\t';

	time_::Timestamp<time_::Microseconds> t;
	std::cerr << t << '\t';
	date::from_stream(ss, input_fmt.c_str(), t.time_point);
	std::cerr << t << '\n';
#endif


#if 0
	getListOfCurrencies();
	auto r = getListOfCurrencies();
	auto t = processListOfCurrencies(std::move(r));
	Print(t, std::cout, &CurrencyData::id, &CurrencyData::name);

//	auto single_currency = processSingleCurrency(getSingleCurrency("XTZ"));
//	cout << single_currency;
//
//	single_currency = processSingleCurrency(getSingleCurrency("ETH"));
//	cout << single_currency;
//
//	single_currency = processSingleCurrency(getSingleCurrency("BTC"));
//	cout << single_currency;

#endif

#if 0
	for (int i = 0; i != 10; ++i) {
		auto trade1 = processSingleTradeData(getSingleTradeData("ETH", "USD"));
		if (not trade1.empty) {
			cout << "ETH-USD: " << trade1.price << ' ' << trade1.volume << ' ' << trade1.time << '\n';
		}
		else {
			cout << "ETH-USD: " << "no data\n";
		}

		auto trade2 = processSingleTradeData(getSingleTradeData("BTC", "USD"));
		if (not trade2.empty) {
			cout << "BTC-USD: " << trade2.price << ' ' << trade2.volume << ' ' << trade2.time << '\n';
		}
		else {
			cout << "BTC-USD: " << "no data\n";
		}

		auto trade3 = processSingleTradeData(getSingleTradeData("ETH", "BTC"));
		if (not trade3.empty) {
			cout << "ETH-BTC: " << trade3.price << ' ' << trade3.volume << ' ' << trade3.time << '\n';
		}
		else {
			cout << "ETH-BTC: " << "no data\n";
		}
	}
#endif
//	std::cout << "Work in progress...\n";


#if 0
	for (int i = 0; i != 10; ++i) {
		auto trade1 = processSingleTradeData(getSingleTradeData("ADA", "ETH"));
		if (not trade1.empty) {
			cout << "ADA-ETH: " << trade1.price << ' ' << trade1.volume << ' ' << trade1.time << '\n';
		}
		else {
			cout << "ADA-ETH: " << "no data\n";
		}

		auto trade3 = processSingleTradeData(getSingleTradeData("ETH", "BTC"));
		if (not trade3.empty) {
			cout << "ETH-BTC: " << trade3.price << ' ' << trade3.volume << ' ' << trade3.time << '\n';
		}
		else {
			cout << "ETH-BTC: " << "no data\n";
		}
	}
#endif


#if 0

//	getContractData(true, std::cout);

	QueryFactory qf;

	curl_client::Response response;
	curl_client::Request request;

	std::cout << "Storage:\n";
	auto handle = qf.getStorageQuery("KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1");
	response = request.
							  pathSetNew(std::move(handle))->
							  Implement(curl_client::Method::Get);
	Print(response, std::cout, false);
	std::cout << '\n';

	TokenPair<Seconds> token;
	token.pair_address = "KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1";
	token = estimateTezToToken(std::move(token));
	std::cout << "tez pool: " << token.tez_pool << "; token_pool: " << token.token_pool << "; price: " << token.current_price << '\n';

	token = estimateTokenToTez(std::move(token));
	std::cout << "tez pool: " << token.tez_pool << "; token_pool: " << token.token_pool << "; price: " << token.current_price << '\n';

#endif

#if 0

	Strategy strategy ("simple_demo");

	///filtered and referenced data
	strategy.addIndicator(
			"ETH-BTC",				//ticker
			"base_contract"			//label
	);
	strategy.addIndicator("ADA-ETH", "other_contract");
//	strategy.addIndicator("C3", "modified_contract", modifier);

	///relations of new market data and filtered/referenced data
	strategy.addSignal(
			"basic_comparison", //label
			"Comparison",		//type of a Signal
			"LT",				//relation between the indicators
			{"base_contract", "other_contract"} //indicators to compare
	);

	//todo: check that this works - check relations
	///actions, based on relations
	strategy.addRule(
			"ETH-BTC"s,						//ticker to generate trades for
			"enter_when_less",				//rule_label
			"Entry",						//type of a Rule
			"basic_comparison",				//signal label
			1,								//value of a signal -1, 0, 1 //todo: convert to TRUE/FALSE
			"Neutral", 						//Position to enter txn
			types::Value(1),
			"Enter", 						//Exit, Riskm Rebalance - required for prioritization if there are several Rules triggering
			"Market"						//how to behave in the market Market/Limited/FillOrKill
	);

	int counter = 0;
	while (true) {
		strategy.getMarketData();
		auto trade = strategy.ruleSignal();
		if (trade.has_value()) {
			cout << trade.value().getTicker() << " quantity to trade: " << trade.value().getQuantity() << '\n';
		}
		else {
			cout << "no signal for a trade\n";
		}

		std::this_thread::sleep_for(1ms);
		cout << "==========\n";
		if (counter++ > 10) break;
	}
#endif


#if 0
	/*
	price diff
	timeframe
	 sizing in terms of Tezos
	 */

	Strategy strategy ("simple_demo");

	///filtered and referenced data
	strategy.addIndicator(
			"ETH-BTC",				//ticker
			"base_contract"			//label
	);
	strategy.addIndicator("ADA-ETH", "other_contract");
//	strategy.addIndicator("C3", "modified_contract", modifier);

	///relations of new market data and filtered/referenced data
	strategy.addSignal(
			"basic_comparison", //label
			"Comparison",		//type of a Signal
			"GT",				//relation between the indicators
			{"base_contract", "other_contract"} //indicators to compare
	);

	//todo: check that this works - check relations
	///actions, based on relations
	strategy.addRule(
			"ETH-BTC"s,						//ticker to generate trades for
			"enter_when_less",				//rule_label
			"Entry",						//type of a Rule
			"basic_comparison",				//signal label
			1,								//value of a signal -1, 0, 1 //todo: convert to TRUE/FALSE
			"Neutral", 						//Position to enter txn
			types::Value(1),
			"Enter", 						//Exit, Riskm Rebalance - required for prioritization if there are several Rules triggering
			"Market"						//how to behave in the market Market/Limited/FillOrKill
	);

	tg_bot::TgBotUI bot;

	int counter = 0;
	while (true) {
		strategy.getMarketData();
		auto trade = strategy.ruleSignal();
		if (trade.has_value()) {
			types::String text;
			text += trade.value().getTicker();
			text += " quantity to trade: ";
			text += trade.value().getQuantity().ToString();
			text += '\n';
			bot.postData(std::move(text));
		}
		else {
			bot.postData("no signal for a trade\n");
		}

		std::this_thread::sleep_for(2s);
		if (counter++ > 10) break;
	}
#endif

#if 0
	tg_bot::TgBotUI bot;
	bot.init();
	bot.run();

#endif


#if 0

	auto& config = algo::config::RobotConfig::getInstance("../setup_user_data/bot_config.ini");
	config.loadFromIni();
	config.printIniFile(std::cerr);
	config.printSettings(std::cerr);

	algo::config::ContractInfo ci;
	ci.name = "name5";
	ci.ticker_cb = "CB 5";
	ci.ticker_qs = "QS 5";
	ci.price_diff = 42;
	ci.timeframe = 42;
	ci.trade_size = 42;
	ci.slippage = 4;

	config.updateIni(std::move(ci));
	config.loadFromIni();
	config.printIniFile(std::cerr);
	config.printSettings(std::cerr);

	auto c5 = config.getContractInfo("name5");
	std::cerr << c5 << '\n';

	auto fuck = config.getContractInfo("fuck");
	std::cerr << fuck << '\n';

#endif


#if 1

	algo::tezos::quipuswap::transaction::makeTransaction(""s);

#endif



	return 0;
}
