#include "coinbase.h"
#include "strategy.h"
#include "randomer.h"

#include <iostream>
#include <iomanip>
#include <array>

#include <chrono>
#include <thread>


using namespace algo;
using namespace std;
using namespace std::literals;
using namespace std::chrono_literals;
using namespace boost;
using namespace algo::coinbase;

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

	auto single_currency = processSingleCurrency(getSingleCurrency("XTZ"));
	cout << single_currency;

	single_currency = processSingleCurrency(getSingleCurrency("ETH"));
	cout << single_currency;

	single_currency = processSingleCurrency(getSingleCurrency("BTC"));
	cout << single_currency;

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
	const std::array<string, 4> tickers {"A1"s, "B2"s, "C3"s, "D4"s};
	DataProcessor data_processor;

	Strategy strategy ("A1"s, "simple_demo", nullptr);

	///filtered and referenced data
	strategy.addIndicator(
			"A1",					//ticker
			"base_contract"			//label
	);
	strategy.addIndicator("B2", "other_contract");
	strategy.addIndicator("C3", "modified_contract", modifier);

	///relations of new market data and filtered/referenced data
	strategy.addSignal(
			"basic_comparison", //label
			"Comparison",		//type of a Signal
			"GT",				//relation between the indicators
			{"base_contract", "modified_contract"} //indicators to compare
	);

	///actions, based on relations
	strategy.addRule(
			"enter_when_greater",			//rule_label
			"Entry",						//type of a Rule
			"basic_comparison",				//signal label
			1,								//value of a signal -1, 0, 1 //todo: convert to TRUE/FALSE
			"Neutral", 						//Position to enter txn
			types::Value(1),
			"Enter", 						//Exit, Riskm Rebalance - required for prioritization if there are several Rules triggering
			"Market"						//how to behave in the market Market/Limited/FillOrKill
	);

	for (int i = 0; i != 10; ++i) {

		for (int j = 0; j < 4; ++j) {
			auto new_ticker = tickers[prm(0, 3)];
			auto new_raw_figure = prm(12.5, 22.5);
			data_processor.addNewMarketData(new_ticker, new_raw_figure);
		}

		auto new_market_data = data_processor.getNewMarketData();
		for (const auto &market_data : new_market_data) {
			cout
			<< market_data.quote.timestamp << ": "
			<< market_data.ticker << " " << market_data.quote << "; ";
			auto trade = strategy.ruleSignal(new_market_data);

			if (trade.has_value()) {
				cout << trade.value().getTicker() << " quantity to trade: " << trade.value().getQuantity() << '\n';
			}
			else {
				cout << "no signal for a trade\n";
			}
		}
		std::this_thread::sleep_for(1ms);
		cout << "==========\n";
	}
#endif

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

#if 1

	Strategy strategy ("ETH-BTC"s, "simple_demo");

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
			{"base_contract", "modified_contract"} //indicators to compare
	);

	///actions, based on relations
	strategy.addRule(
			"enter_when_greater",			//rule_label
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
	return 0;
}
