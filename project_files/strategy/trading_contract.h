//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "object_type.h"
#include "types_declarations.h"

#ifndef STRATEGY_TRADING_TRADING_CONTRACT_H
#define STRATEGY_TRADING_TRADING_CONTRACT_H

namespace algo {

  using Ticker = types::String;

  namespace trading_contract_base {

	namespace dex_source {
	  struct Quipuswap {};
	  struct Coinbase {};
	}

	using DexSource =
	std::variant<
			std::monostate,
			dex_source::Quipuswap,
			dex_source::Coinbase
	>;

	struct Dex final : public types::ObjectType<DexSource> {};

	types::String DexToString (const Dex &type);
	Dex StringToDex (const types::String& type);
	Dex TickerToDex (Ticker ticker);

	namespace quipuswap {
	  struct SellXTZBuyToken{};
	  struct BuyXTZSellToken{};

	  using QuipuswapTradeSideBase =
	  std::variant<
			  std::monostate,
			  SellXTZBuyToken,
			  BuyXTZSellToken
	  >;

	  struct FA1_2{};
	  struct FA2 {};
	  using FactoryBase =
	  std::variant<
			  std::monostate,
			  FA1_2,
			  FA2
	  >;

	}//!namespace

	struct QiupuswapTradeSide final : public types::ObjectType<quipuswap::QuipuswapTradeSideBase> {};
	struct QiupuswapFactory final : public types::ObjectType<quipuswap::FactoryBase> {};


	types::String QiupuswapTradeSideToString (const QiupuswapTradeSide &type);
	QiupuswapTradeSide StringToQiupuswapTradeSide (const types::String& type);

  }//!namespace

	//todo: THIS SHOULD BE UPDATE TO THE NORMAL QUALITY, NOW IT IS NOT
  struct TradingContract final {
	  const Ticker ticker;
	  const trading_contract_base::Dex dex;
	  trading_contract_base::QiupuswapTradeSide quipuswap_trade_side;

	  trading_contract_base::QiupuswapFactory factory;
	  bool is_factory_set = false;
	  Ticker trading_ticker;

	  explicit TradingContract (Ticker t);
	  explicit TradingContract (Ticker t, types::String trade_side);
  };

  bool operator < (const TradingContract& lhs, const TradingContract& rhs);


  }//!namespace

#endif //STRATEGY_TRADING_TRADING_CONTRACT_H
