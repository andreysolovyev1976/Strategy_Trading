//
// Created by Andrey Solovyev on 14/10/2021.
//

#include <gtest/gtest.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
	testing::InitGoogleTest(&argc, argv);
	testing::GTEST_FLAG(color) = "yes";
	::testing::GTEST_FLAG(repeat) = 10;
//	::testing::GTEST_FLAG(filter) = "Utf8JsonWriter*";
	::testing::GTEST_FLAG(filter) = "Strategy.SingleStrategy";
//	::testing::GTEST_FLAG(filter) = "ThreadSafeSet*";

	return RUN_ALL_TESTS();
}