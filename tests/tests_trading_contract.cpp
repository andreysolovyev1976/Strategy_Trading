//
// Created by Andrey Solovyev on 04/11/2021.
//

#include <gtest/gtest.h>
#include "trading_contract.h"


TEST(TradingContract, DexToString) {
	using namespace algo::trading_contract_base;
	const auto qs = DexToString(Dex{dex_source::Quipuswap{}});
	ASSERT_EQ(qs, "Quipuswap");

	const auto cb = DexToString(Dex{dex_source::Coinbase{}});
	ASSERT_EQ(cb, "Coinbase");

	auto empty = DexToString(Dex{});
	ASSERT_TRUE(empty.empty());
}

TEST(TradingContract, StringToDex) {
	using namespace algo::trading_contract_base;
	auto qs = StringToDex("Quipuswap");
	ASSERT_TRUE(qs.TryAs<dex_source::Quipuswap>());

	auto cb = StringToDex("Coinbase");
	ASSERT_TRUE(cb.TryAs<dex_source::Coinbase>());

	auto empty = StringToDex("UNKNOWN");
	ASSERT_TRUE(empty.TryAs<std::monostate>());
}

TEST(TradingContract, TickerToDex) {
	using namespace algo;
	using namespace algo::trading_contract_base;
	Ticker ticker_qs ("KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT"); //test net Hangzhounet
	Ticker ticker_cb ("XTZ-BTC");
	Ticker ticker_empty ("UNKNOWN");

	auto qs = TickerToDex(ticker_qs);
	ASSERT_TRUE(qs.TryAs<dex_source::Quipuswap>());

	auto cb = TickerToDex(ticker_cb);
	ASSERT_TRUE(cb.TryAs<dex_source::Coinbase>());

	auto empty = TickerToDex(ticker_empty);
	ASSERT_TRUE(empty.TryAs<std::monostate>());
}


TEST(TradingContract, QiupuswapTradeSideToString){
	using namespace algo::trading_contract_base;
	const auto qs = QiupuswapTradeSideToString(QiupuswapTradeSide{quipuswap::SellXTZBuyToken{}});
	ASSERT_EQ(qs, "SellXTZBuyToken");

	const auto cb = QiupuswapTradeSideToString(QiupuswapTradeSide{quipuswap::BuyXTZSellToken{}});
	ASSERT_EQ(cb, "BuyXTZSellToken");

	auto empty = QiupuswapTradeSideToString(QiupuswapTradeSide{});
	ASSERT_TRUE(empty.empty());
}

TEST(TradingContract, StringToQiupuswapTradeSide){
	using namespace algo::trading_contract_base;
	auto qs = StringToQiupuswapTradeSide("SellXTZBuyToken");
	ASSERT_TRUE(qs.TryAs<quipuswap::SellXTZBuyToken>());

	auto cb = StringToQiupuswapTradeSide("BuyXTZSellToken");
	ASSERT_TRUE(cb.TryAs<quipuswap::BuyXTZSellToken>());

	auto empty = StringToQiupuswapTradeSide("UNKNOWN");
	ASSERT_TRUE(empty.TryAs<std::monostate>());
}


TEST(TradingContract, ContractCoinbase) {
	using namespace algo;
	Ticker ticker ("XTZ-BTC"); //test net Hangzhounet
	TradingContract trading_contract (ticker);

	ASSERT_TRUE(trading_contract.dex.TryAs<trading_contract_base::dex_source::Coinbase>());
}

TEST(TradingContract, ContractQuipuswap) {
	using namespace algo;
	Ticker ticker ("KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT");
	TradingContract trading_contract (ticker);

	ASSERT_TRUE(trading_contract.dex.TryAs<trading_contract_base::dex_source::Quipuswap>());
}

TEST(TradingContract, ContractQuipuswapWithTradeSide) {
	using namespace algo;
	using namespace algo::trading_contract_base;

	Ticker ticker ("KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT");
	TradingContract trading_contract_1 (ticker, "SellXTZBuyToken");
	ASSERT_TRUE(trading_contract_1.dex.TryAs<trading_contract_base::dex_source::Quipuswap>());
	ASSERT_TRUE(trading_contract_1.quipuswap_trade_side.TryAs<quipuswap::SellXTZBuyToken>());

	TradingContract trading_contract_2 (ticker, "BuyXTZSellToken");
	ASSERT_TRUE(trading_contract_2.dex.TryAs<trading_contract_base::dex_source::Quipuswap>());
	ASSERT_TRUE(trading_contract_2.quipuswap_trade_side.TryAs<quipuswap::BuyXTZSellToken>());
}
