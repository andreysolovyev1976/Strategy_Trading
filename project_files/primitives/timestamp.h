//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include <ctime>
#include <chrono>
#include <ratio>
#include <cstdint>
#include <iosfwd>
#include "date/date.h"

#ifndef STRATEGY_TRADING_TIMESTAMP_H
#define STRATEGY_TRADING_TIMESTAMP_H

namespace algo {
  namespace time_ {

    //todo: add is_convertible to duration

	using Clock = std::chrono::system_clock;
	using HiResClock = std::chrono::high_resolution_clock;

	using Microseconds = std::chrono::microseconds;
	using Milliseconds = std::chrono::milliseconds;
	using Seconds = std::chrono::seconds;
	using Minutes = std::chrono::minutes;
	using FiveMinutes = std::chrono::duration<long, std::ratio<300>>;
	using ThirtySeconds = std::chrono::duration<long, std::ratio<30>>;

	/*
typedef duration<long long,         nano> nanoseconds;
typedef duration<long long,        micro> microseconds;
typedef duration<long long,        milli> milliseconds;
typedef duration<long long              > seconds;
typedef duration<     long, ratio<  60> > minutes;
typedef duration<     long, ratio<3600> > hours;
 */

	//todo: add SFINAE / concept to check Duration is a time constructable


	template <typename Duration>
	constexpr bool IsMicroseconds (Duration) {
		return std::is_same_v<Duration, Microseconds>;
	}
	template <typename Duration>
	constexpr bool IsMilliseconds (Duration) {
		return std::is_same_v<Duration, Milliseconds>;
	}
	template <typename Duration>
	constexpr bool IsSeconds (Duration) {
		return std::is_same_v<Duration, Seconds>;
	}
	template <typename Duration>
	constexpr bool IsMinutes (Duration) {
		return std::is_same_v<Duration, Minutes>;
	}


	template<class Duration>
	using TimePoint = std::chrono::time_point<Clock, Duration>;

	template<class Duration>
	struct Timestamp {
		Timestamp() : time_point(std::chrono::floor<Duration>(Clock::now()))
		{}

		TimePoint<Duration> time_point;

		types::String toString () const {
			types::String output;
			output.reserve(24);
			output += date::format("%F", time_point);
			output += ' ';
			output += date::format("%T", time_point);
			return output;
		}
	};

	template <typename Duration>
	struct TimestampHasher {
		size_t operator () (const Timestamp<Duration>& t) const {
			return t.time_point.time_since_epoch().count();
		}
	};

	template<class Duration>
	bool operator==(const Timestamp<Duration>& lhs, const Timestamp<Duration>& rhs) {
		return lhs.time_point == rhs.time_point;
	}
	template<class Duration>
	bool operator!=(const Timestamp<Duration>& lhs, const Timestamp<Duration>& rhs){
		return not (rhs==lhs);
	}
	template<class Duration>
	bool operator < (const Timestamp<Duration>& lhs, const Timestamp<Duration>& rhs){
		return lhs.time_point < rhs.time_point;
	}
	template<class Duration>
	bool operator >(const Timestamp<Duration>& lhs, const Timestamp<Duration>& rhs){
		return not (rhs < lhs);
	}
	template<class Duration>
	bool operator<=(const Timestamp<Duration>& lhs, const Timestamp<Duration>& rhs){
		return not (rhs > lhs);
	}
	template<class Duration>
	bool operator>=(const Timestamp<Duration>& lhs, const Timestamp<Duration>& rhs) {
		return not (lhs<rhs);
	}

	template<class Duration>
	std::ostream& operator<<(std::ostream& os, const Timestamp<Duration>& timestamp){
		os << timestamp.toString();
		return os;
	}

	template<class Duration>
	Timestamp<Duration> fromString(types::String input, const types::String& input_fmt){
		Timestamp<Duration> timestamp;
		std::stringstream ss(std::move(input));
		date::from_stream(ss, input_fmt.c_str(), timestamp.time_point);
		return timestamp;
	}


	template<class Duration>
	Timestamp<Duration> operator - (const Timestamp<Duration>& lhs, const Timestamp<Duration>& rhs){
		Timestamp<Duration> result;
		result.time_point -= std::chrono::duration_cast<Duration>(lhs.time_point - rhs.time_point);
		return result;
	}

	//todo: unfinished
	template<class Duration>
	Timestamp<Duration> operator + ([[maybe_unused]] const Timestamp<Duration>& lhs, [[maybe_unused]] const Timestamp<Duration>& rhs){
		Timestamp<Duration> result;
//		result.time_point += std::chrono::duration_cast<Duration>(lhs.time_point + rhs.time_point);
		return result;
	}

  }//!namespace
}//!namespace

#endif //STRATEGY_TRADING_TIMESTAMP_H
