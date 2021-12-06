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

algo::Quote modifier (const algo::Quote& quote){
	Quote modified = quote;
	modified.value = quote.value - types::Value(10);
	return modified;
}

int main() {

	//todo: Check The Rules - add tickers
	//todo: check Indicator and Signal updates
	//todo: Add ThreadSafe maps
	//todo: Add Map size limit


	//todo: add a case when there is a new data for some of the labels in strategy - probably check that when ruleSignaling

//	std::cout << "Work in progress...\n";

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
			<< market_data.ticker << " " << market_data.quote.value << "; ";
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
	return 0;
}
