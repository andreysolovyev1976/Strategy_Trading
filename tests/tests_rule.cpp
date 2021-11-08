//
// Created by Andrey Solovyev on 02/11/2021.
//

#include <gtest/gtest.h>
#include "rule.h"
#include <iostream>

TEST(Rule, Primitives) {
	using namespace algo;
	using namespace algo::rule_base::rule_type;

	algo::Rule s1 ({Buy{}});
	algo::Rule s2 ({Sell{}});
	algo::Rule s3 ({DoNothing{}});

	ASSERT_TRUE(s1.Is<Buy>());
	ASSERT_TRUE(not s1.Is<Sell>());
	ASSERT_TRUE(not s1.Is<DoNothing>());
	ASSERT_TRUE(s2.Is<Sell>());
	ASSERT_TRUE(not s2.Is<Buy>());
	ASSERT_TRUE(not s2.Is<DoNothing>());
	ASSERT_TRUE(s3.Is<DoNothing>());
	ASSERT_TRUE(not s3.Is<Buy>());
	ASSERT_TRUE(not s3.Is<Sell>());

}