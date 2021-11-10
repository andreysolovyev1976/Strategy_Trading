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
	Quote modified;
	modified.timestamp = quote.timestamp;
	modified.value = quote.value - types::Value(10);
	return modified;
}

int main() {
	//todo: finalize operators for Value
	//todo: add a case when there is a new data for some of the labels in strategy - probably check that when ruleSignaling

//	std::cout << "Work in progress...\n";

	const std::array<string, 4> tickers {"A1"s, "B2"s, "C3"s, "D4"s};

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
			{"base_contract", "other_contract"} //indicators to compare
	);

	///actions, based on relations
	strategy.addRule(
			"enter_when_greater",			//rule_label
			"Entry",						//type of a Rule
			"basic_comparison",				//signal label
			1,								//value of a signal -1, 0, 1 //todo: convert to TRUE/FALSE
			"Neutral", 						//Position to enter txn //todo: convert from a String
			types::Value(1),
			"Enter", 						//Exit, Riskm Rebalance - required for prioritization if there are several Rules triggering
			"Market"						//how to behave in the market Market/Limited/FillOrKill
	);

	for (int i = 0; i != 10; ++i) {

		auto new_ticker = tickers[prm(0, 3)];
		auto new_raw_figure = prm(12.5, 22.5);

		auto new_market_data = DataProcessor::getNewMarketData(new_ticker, new_raw_figure);
		cout << new_market_data.quote.timestamp << ": " << new_market_data.ticker << " " << setw(11) << setprecision(10)
			 << new_raw_figure << "; ";
		auto trade = strategy.ruleSignal(new_market_data);

		if (trade.has_value()) {
			cout << trade.value().getTicker() << " quantity to trade: " << trade.value().getQuantity() << '\n';
		}
		else {
			cout << "no signal for a trade\n";
		}
	}
	return 0;
}
