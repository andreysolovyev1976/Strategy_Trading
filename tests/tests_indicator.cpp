//
// Created by Andrey Solovyev on 04/11/2021.
//

#include <gtest/gtest.h>
#include "indicator.h"
#include <sstream>

types::Value doubleThis (types::Value value) {
	return value + value;
}

types::Value addTwelve (types::Value value) {
	return value + types::Value(12);
}


TEST(Indicator, Value_Modifier) {
	using namespace std::literals;

//	types::Value value(0.15, 2);
//	algo::Indicator indicator("test"s, std::move(value), doubleThis);
//
//	std::stringstream str;
//	str << indicator;
//	ASSERT_EQ(str.str(), "label: test indicator: 0.30") << indicator.getOutputValues();
}

TEST(Indicator, Value_NoModifier) {
	using namespace std::literals;

//	types::Value value(0.15, 2);
//	algo::Indicator<types::Value> indicator ("test"s, std::move(value));
//
//	std::stringstream str;
//	str << indicator;
//	ASSERT_EQ(str.str(), "label: test indicator: 0.15");
}

TEST(Indicator, Quote_Modifier) {
}

TEST(Indicator, Quote_NoModifier) {
}

TEST(Indicator, Value_Vec_Modifier) {
}

TEST(Indicator, Value_Vec_NoModifier) {
}
TEST(Indicator, Quote_Vec_Modifier) {
}

TEST(Indicator, Quote_Vec__NoModifier) {
}