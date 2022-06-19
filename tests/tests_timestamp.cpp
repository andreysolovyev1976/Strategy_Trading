//
// Created by Andrey Solovyev on 04/11/2021.
//

#include <gtest/gtest.h>
#include "timestamp.h"
#include <sstream>

using namespace algo::time_;
using namespace std::chrono_literals;

template <typename Duration1, typename Duration2, typename ResultDuration>
Timestamp<ResultDuration> operation ([[maybe_unused]] Timestamp<Duration1> lhs, [[maybe_unused]] Timestamp<Duration2> rhs) {
	return Timestamp<ResultDuration>{};
}

TEST(Timestamp, Timer) {

	auto stop = std::chrono::system_clock::now() + 7s;
	auto time_left = std::chrono::duration_cast<Seconds>(stop - std::chrono::system_clock::now()).count();

	while (time_left > 0) {
		std::this_thread::sleep_for(3s);
		time_left = std::chrono::duration_cast<Seconds>(stop - std::chrono::system_clock::now()).count();
		std::cerr << time_left << '\n';
	}
}


TEST(Timestamp, ComparisonOk) {
}
TEST(Timestamp, ComparisonNotOk) {
}
TEST(Timestamp, Hasher) {
}
TEST(Timestamp, Add) {
	Timestamp<Seconds> t1;
	std::this_thread::sleep_for(2s);
	Timestamp<Seconds> t2;
	std::cerr << (t2 + t1) << '\n';
}
TEST(Timestamp, Subtract) {
	Timestamp<Seconds> t1;
	std::this_thread::sleep_for(2s);
	Timestamp<Seconds> t2;
	std::cerr << (t2 - t1) << '\n';
}
TEST(Timestamp, Multiply) {
}
TEST(Timestamp, Divide) {
}



TEST(Timestamp, Print) {

	Timestamp<Seconds> t1;
	std::this_thread::sleep_for(2s);
	Timestamp<Seconds> t2;

	std::stringstream ss;
	ss << t1 << ' ' << t2;
	ASSERT_EQ(ss.str(), "");

}
