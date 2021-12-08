//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include <ctime>
#include <chrono>
#include <cstdint>
#include <iosfwd>
#include "date/date.h"

#ifndef STRATEGY_TRADING_TIMESTAMP_H
#define STRATEGY_TRADING_TIMESTAMP_H

namespace algo {
  namespace time_ {

	using Clock = std::chrono::system_clock;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::milliseconds;
	using Seconds = std::chrono::seconds;

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


  }//!namespace
}//!namespace

#endif //STRATEGY_TRADING_TIMESTAMP_H
