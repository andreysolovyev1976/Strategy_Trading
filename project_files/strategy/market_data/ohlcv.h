//
// Created by Andrey Solovyev on 19/06/2022.
//

#pragma once

#include "single_quote.h"

#ifndef STRATEGY_TRADING_OHLCV_H
#define STRATEGY_TRADING_OHLCV_H

namespace algo {
  namespace quote_base {
	template <typename Duration>
	class OHLCV final {
	public:
		OHLCV() = default;

		void pushValue (SingleQuote<Duration> single_quote) noexcept;
		void pushValue (const SingleQuote<Duration> &single_quote) noexcept;
		void pushValue (SingleQuote<Duration> &&single_quote) noexcept;

		const size_t getDataSize () const noexcept;
		bool isDataEmpty () const noexcept;
		const SingleQuote<Duration>& getOpen() const noexcept;
		const SingleQuote<Duration>& getHigh() const noexcept;
		const SingleQuote<Duration>& getLow() const noexcept;
		const SingleQuote<Duration>& getClose() const noexcept;
		const SingleQuote<Duration>& getVolume() const noexcept;
		const time_::Timestamp<Duration>& getTimestamp () const noexcept;

	private:
		size_t high_idx{0}, low_idx{0};
		types::Value volume{0};
		std::vector<SingleQuote<Duration>> data; //todo: arbitrary data selected

		bool checkDuration () const;
		void updateExtremes (const SingleQuote<Duration> &single_quote) noexcept;
	};

	template <typename Duration>
	void OHLCV<Duration>::pushValue (SingleQuote<Duration> single_quote) noexcept {
		updateExtrems(single_quote);
		volume += single_quote.volume;
		data.template emplace_back(std::move(single_quote));
	}
	template <typename Duration>
	void OHLCV<Duration>::pushValue (const SingleQuote<Duration> &single_quote) noexcept {
		updateExtrems(single_quote);
		volume += single_quote.volume;
		data.template emplace_back(single_quote);
	}
	template <typename Duration>
	void OHLCV<Duration>::pushValue (SingleQuote<Duration> &&single_quote) noexcept {
		updateExtrems(single_quote);
		volume += single_quote.volume;
		data.template emplace_back(std::move(single_quote));
	}
	template <typename Duration>
	const size_t OHLCV<Duration>::getDataSize() const noexcept {
		return data.size();
	}
	template <typename Duration>
	bool OHLCV<Duration>::isDataEmpty() const noexcept {
		return data.empty();
	}
	template <typename Duration>
	const SingleQuote<Duration>& OHLCV<Duration>::getOpen() const noexcept {
		return isDataEmpty() ? 0 : *data.begin();
	}
	template <typename Duration>
	const SingleQuote<Duration>& OHLCV<Duration>::getHigh() const noexcept {
		return isDataEmpty() ? 0 : data[high_idx];
	}
	template <typename Duration>
	const SingleQuote<Duration>& OHLCV<Duration>::getLow() const noexcept {
		return isDataEmpty() ? 0 : data[low_idx];
	}
	template <typename Duration>
	const SingleQuote<Duration>& OHLCV<Duration>::getClose() const noexcept {
		return isDataEmpty() ? 0 : data.back();
	}
	template <typename Duration>
	const SingleQuote<Duration>& OHLCV<Duration>::getVolume() const noexcept {
		return volume;
	}
	template <typename Duration>
	const time_::Timestamp<Duration>& OHLCV<Duration>::getTimestamp () const noexcept {
		return data[close_idx].timestamp;
	}

	template <typename Duration>
	bool OHLCV<Duration>::checkDuration () const {}

	template <typename Duration>
	void OHLCV<Duration>::updateExtremes (const SingleQuote<Duration> &single_quote) noexcept {
		if (data.empty()) return;
	
		if (single_quote.value > data[high_idx]) {
			high_idx = data.size();
		}
		else if (single_quote.value < data[low_idx]) {
			low_idx = data.size();
		}
	}
  }//!namespace
}//!namespace


#endif //STRATEGY_TRADING_OHLCV_H
