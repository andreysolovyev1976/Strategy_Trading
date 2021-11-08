//
// Created by Andrey Solovyev on 03/11/2021.
//

#include <gtest/gtest.h>

#include <iostream>
#include "value.h"
#include <boost/multiprecision/cpp_int.hpp>

template<typename T, types::IsInteger<T> = true>
constexpr bool is_data(T&&){
	return true;
}//!func

template<typename T, types::IsNotInteger<T> = true>
constexpr bool is_not_data(T&&){
	return true;
}//!func

int some_func() {
	using namespace boost::multiprecision;

//	std::cerr << std::numeric_limits<int128_t>::min() << ' ' << std::numeric_limits<uint128_t>::max() << '\n';
	[[maybe_unused]] int128_t b (1);
	[[maybe_unused]] uint128_t u (1);
	[[maybe_unused]] double d (42.);

	if constexpr (is_data(b)) {} // std::cout << typeid(b).name() << '\n';
	if constexpr (is_data(u)) {} // std::cout << typeid(b).name() << '\n';
//		if constexpr (is_data(d)) std::cout << typeid(b).name() << '\n'; //doesn't compile
//		if constexpr (is_not_data(b)) std::cout << typeid(b).name() << '\n';  //doesn't compile
//		if constexpr (is_not_data(u)) std::cout << typeid(b).name() << '\n';  //doesn't compile
	if constexpr (is_not_data(d)) {} // std::cout << typeid(b).name() << '\n';

	return 0;
}


TEST(Value, Types) {
	using namespace std::literals;

	ASSERT_TRUE(some_func() == 0);
	[[maybe_unused]] types::Value i (42);
	[[maybe_unused]] types::Value iu (42u);
	[[maybe_unused]] types::Value d (42.0, types::ShortUInt(2));
	[[maybe_unused]] types::Value bi (types::BigInt(42));
	[[maybe_unused]] types::Value bu (types::BigUInt(42));

//	[[maybe_unused]] types::Value d_err (42.); //doesn't compile
//	[[maybe_unused]] types::Value s_err ("42"s); //doesn't compile

	ASSERT_TRUE(true);
}
