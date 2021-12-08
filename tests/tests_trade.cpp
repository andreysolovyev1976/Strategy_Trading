//
// Created by Andrey Solovyev on 04/11/2021.
//

#include <gtest/gtest.h>
#include "trade.h"
#include <iostream>

TEST(Trade, Primitives) {

	using namespace algo::time_;
	using namespace algo::trade_base;
	using namespace algo::trade_base::trade_type;
	using namespace algo::trade_base::order_type;
	using namespace std::literals;

	/*
	Trade trade ("TK",
			types::Value(1000),
			Quote( Timestamp("timestamp"s), types::Value(6.25, types::ShortUInt(2)) ),
			Long{},
			Enter{},
			Market{}
			);
			*/
}