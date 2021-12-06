//
// Created by Andrey Solovyev on 03/11/2021.
//

#include <gtest/gtest.h>

#include "value.h"
#include <sstream>


TEST(Value, Ctors) {
	using namespace std::literals;

	[[maybe_unused]] types::Value i (42);
	[[maybe_unused]] types::Value iu (42u);
	[[maybe_unused]] types::Value d1 (42.0, 2);
	[[maybe_unused]] types::Value d2 (42.);
	[[maybe_unused]] types::Value F (types::Float (42.));
	[[maybe_unused]] types::Value bi (types::BigInt(42));
	[[maybe_unused]] types::Value bu (types::BigUInt(42));

	[[maybe_unused]] types::Value s2 ("42"s);
	ASSERT_THROW(types::Value s1 ("fuck"s), std::exception);

	//todo: add Json::Node ctors

	ASSERT_TRUE(true);
}

TEST(Value, ComparisonOperators) {
	using namespace std::literals;
	types::Value v1 (-1.5, 1), v2(2.5, 2), v3(1.0), v4(-1.0), v5(-2.5, 3);
	ASSERT_TRUE(v1 < v2);
	ASSERT_TRUE(v1 <= v1);
	ASSERT_TRUE(v1 <= v2);
	ASSERT_TRUE(v2 > v1);
	ASSERT_TRUE(v2 >= v2);
	ASSERT_TRUE(v2 >= v1);
	ASSERT_TRUE(v2 != v1);
	ASSERT_TRUE(v1 != v2);
	ASSERT_TRUE(v1 == v1);
	ASSERT_TRUE(v2 == v2);
}

TEST(Value, BinaryOperators) {
	using namespace std::literals;
	types::Value v1 (-1.5, 1), v2(2.5, 2), v3(1.0), v4(-1.0), v5(-2.5, 3);

	ASSERT_TRUE(v2 == v2 * v3);
	ASSERT_TRUE(v2 == v2 / v3);
	ASSERT_TRUE(v5 == v2 * v4);
	ASSERT_TRUE(v5 == v2 / v4);
	ASSERT_TRUE(v3 == v1 + v2);
	ASSERT_TRUE(v3 == v1 - v5);
}

TEST(Value, AssignmentOperators) {
	using namespace std::literals;
	types::Value v1 (-1.5, 1), v2(2.5, 2), v3(1.0), v4(-1.0), v5(-2.5, 3);
	v1 -= v5;
	ASSERT_TRUE(v3 == v1);
	v1 += v4;
	ASSERT_TRUE(0 == v1);

	v1 += 2.5;
	v1 *= v3;
	ASSERT_TRUE(v2 == v1);

	v2 /= v5;
	ASSERT_TRUE(v2 == v4);
}

TEST(Value, PrintWithPrecision) {
	using namespace std::literals;
	types::Value v1 (-1.0000000001, 10), v2(2.500000000012, 13), v3(-2.5, 4), v4(-2.126, 2);
	std::stringstream output;

	output << v1;
	ASSERT_EQ (output.str(), "-1.0000000001");
	output.str(std::string());

	output << v2;
	ASSERT_EQ (output.str(), "2.5000000000120");
	output.str(std::string());

	output << v3;
	ASSERT_EQ (output.str(), "-2.5000");
	output.str(std::string());

	output << v4;
	ASSERT_EQ (output.str(), "-2.13");
	output.str(std::string());
}
