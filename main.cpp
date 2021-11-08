#include <iostream>
#include <iomanip>
#include "randomer.h"
#include "account.h"
#include "data_processor.h"
#include "portfolio.h"

#include <boost/circular_buffer.hpp>
#include <boost/circular_buffer.hpp>

using namespace algo;
using namespace std;
using namespace boost;


int main() {
	//todo: finalize operators for Value

//	std::cout << "Work in progress...\n";


/*
	const array<string, 4> tickers {"A1", "B2", "C3", "D4"};

	Strategy strategy;
	strategy.addIndicator();
	strategy.addSignal();
	strategy.addRule();

	Portfolio portfolio;
	portfolio.addStrategy(strategy);

	for (int i = 0; i != 10; ++i) {
		auto new_ticker = tickers[prm(0, 3)];
		auto new_raw_figure = prm(12.5, 22.5);
		cout << new_ticker << ": " << setprecision(10) << new_raw_figure << '\n';

		auto new_value = DataProcessor::toValue(new_raw_figure);

		strategy.ruleSignal(new_value);
		portfolio.getPosition();

	}
*/
	return 0;
}
