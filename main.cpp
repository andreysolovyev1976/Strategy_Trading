#include "coinbase.h"
#include "tzkt_io.h"
#include "quipuswap.h"
#include "strategy.h"
#include "randomer.h"

#include "ui.h"

#include <iostream>
#include <iomanip>
#include <array>
#include <chrono>
#include <thread>

#include "bot_config.h"
#include "dex_transaction.h"
#include "strategy_engine.h"

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

	//todo: check Indicator and Signal updates
	//todo: Add Value ctor from Json
	//todo: Add Logging at large
	//todo: Manage threads - it seems like a leak with no proper interruption
	//todo: add a case when there is a new data for some of the labels in strategy - probably check that when ruleSignaling

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
	TgBot::Bot bot (const_values::TG_BOT_TOKEN);
	algo::Engine engine (&bot);

	///filtered and referenced data
	Indicator i1(
			"base_contract",		//label
			"ETH-BTC",				//ticker
			"SellXTZBuyToken"
	);

	Indicator i2("other_contract", "ETH-USD", "BuyXTZSellToken");
	Indicator i3 ("quipuswap_contract", "KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1", "SellXTZBuyToken");
	engine.addTradingObject(std::move(i1));
	engine.addTradingObject(std::move(i2));
	engine.addTradingObject(std::move(i3));

	///relations of new market data and filtered/referenced data
	Signal s1(
			"basic_comparison", //label
			"Comparison",		//type of a Signal
			"LT",				//relation between the indicators
			{"base_contract", "other_contract"}, //indicators to compare
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(s1));

	Signal s2(
			"qs_vs_cb", 		//label
			"Comparison",		//type of a Signal
			"LT",				//relation between the indicators
			{"other_contract", "quipuswap_contract"}, //indicators to compare
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(s2));

	Signal s3(
			"threshhold_test", 	//label
			"Threshold",		//type of a Signal
			"GT",				//relation between the indicator and threshold
			{"other_contract", "quipuswap_contract"}, //indicators to compare
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(s3));

	Signal s4(
			"equality_test", 	//label
			"Comparison",		//type of a Signal
			"EQ",				//relation between the indicator and threshold
			{"other_contract", "other_contract"}, //indicators to compare
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(s4));


	//todo: check that this works - check relations
	///actions, based on relations
	Rule r1 (
			"enter_when_less",				//rule_label
			"KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1"s,						//ticker to generate trades for
			"SellXTZBuyToken",
			"basic_comparison",				//signal label
			"True",							//value of a signal -1, 0, 1 //todo: convert to TRUE/FALSE
			42,
			engine.getPtr<Signals>()
	);
	engine.addTradingObject(std::move(r1));

	Rule r2 (
			"enter_when_greater",			//rule_label
			"ETH-USD"s,						//ticker to generate trades for
			"BuyXTZSellToken",
//			"Exit",							//type of a Rule
			"qs_vs_cb",						//signal label
			"True",							//value of a signal -1, 0, 1 //todo: convert to TRUE/FALSE
//			"Neutral", 						//Position to enter txn
			types::Value(1),
//			"Enter", 						//Exit, Riskm Rebalance - required for prioritization if there are several Rules triggering
//			"Market",						//how to behave in the market Market/Limited/FillOrKill
			engine.getPtr<Signals>()
	);
	engine.addTradingObject(std::move(r2));

	Rule r3 (
			"enter_tokens",			//rule_label
			"KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT"s,						//ticker to generate trades for
			"SellXTZBuyToken",
			"equality_test",						//signal label
			"True",
			types::Value(1000),
			engine.getPtr<Signals>()
	);
	engine.addTradingObject(std::move(r3));

	Rule r4 (
			"exit_tokens",			//rule_label
			"KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT"s,						//ticker to generate trades for
			"BuyXTZSellToken",
			"equality_test",						//signal label
			"True",
			types::Value(2000),
			engine.getPtr<Signals>()
	);
	engine.addTradingObject(std::move(r4));


	Strategy st1 ("simple_demo 1"s, engine.getPtr<Indicators>(), engine.getPtr<Signals>(), engine.getPtr<Rules>(), 442233888);
	st1.addRule("enter_when_less");
	engine.addTradingObject(std::move(st1));

	Strategy st2 ("simple_demo 2"s, engine.getPtr<Indicators>(), engine.getPtr<Signals>(), engine.getPtr<Rules>(), 442233888);
	st2.addRule("enter_when_greater");
	engine.addTradingObject(std::move(st2));

	Strategy st3 ("tez_to_token"s, engine.getPtr<Indicators>(), engine.getPtr<Signals>(), engine.getPtr<Rules>(), 442233888);
	st3.addRule("enter_tokens");
	engine.addTradingObject(std::move(st3));

	Strategy st4 ("token_to_tez"s, engine.getPtr<Indicators>(), engine.getPtr<Signals>(), engine.getPtr<Rules>(), 442233888);
	st4.addRule("exit_tokens");
	engine.addTradingObject(std::move(st4));
/*
	std::cout << "two strategies\n";
	engine.activateStrategy("simple_demo 1"s);
	engine.activateStrategy("simple_demo 2"s);
	std::this_thread::sleep_for(30s);
	std::cout << "one strategy\n";
	engine.deactivateStrategy("simple_demo 1"s);
	std::this_thread::sleep_for(30s);
	engine.deactivateStrategy("simple_demo 2"s);
	std::cout << "no strategy\n";
	std::this_thread::sleep_for(30s);
	std::cout << "ok\n";
	std::cout << "one strategy\n";
	engine.activateStrategy("simple_demo 1"s);
	std::this_thread::sleep_for(30s);
*/

	engine.activateStrategy("simple_demo 1"s);
	engine.activateStrategy("simple_demo 2"s);
	std::cout << "test there and back\n";
	engine.activateStrategy("tez_to_token"s);
	std::this_thread::sleep_for(2s);
	engine.activateStrategy("token_to_tez"s);
	std::this_thread::sleep_for(90s);

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


#if 0

	using namespace types;
	using namespace algo::tezos::quipuswap::transaction;

	String result;

	os::invokeCommandAndCaptureResult(
			makeCommand("KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT").c_str(),
			result);
	std::cerr << result << '\n';

//	ts-node ../setup_transacton_ts_script/test_transaction.ts --sourcePath=KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT

#endif


#if 1
	user_interface::UI ui;
	ui.run();
#endif

	return 0;
}

//todo:

/*
 SIGNAL 2.2) check comment at 104
RULE 5) figure out where to put trading details - ie Slipage and others
8) check data race

 */
