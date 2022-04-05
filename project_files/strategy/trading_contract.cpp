//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "trading_contract.h"
#include <algorithm>

namespace algo {
  namespace trading_contract_base {

	types::String DexToString (const Dex &type) {
		if      (type.TryAs<dex_source::Quipuswap>()) return "Quipuswap";
		else if (type.TryAs<dex_source::Coinbase>()) return "Coinbase";
		else return "";
	}
	Dex StringToDex (const types::String& type) {
		if      (type == "Quipuswap")return Dex{dex_source::Quipuswap{}};
		else if (type == "Coinbase")return Dex{dex_source::Coinbase{}};
		else return Dex{};
	}
	Dex TickerToDex (Ticker ticker) {
		if (auto space =
				std::find_if(std::begin(ticker), std::end(ticker), [](char c){return c == '-';});
				space != std::end(ticker)) {
			return Dex{dex_source::Coinbase{}};
		} else {
			return Dex{dex_source::Quipuswap{}};
		}
	}

	types::String QiupuswapTradeSideToString (const QiupuswapTradeSide &type ) {
		if      (type.TryAs<quipuswap::SellXTZBuyToken>()) return "SellXTZBuyToken";
		else if (type.TryAs<quipuswap::BuyXTZSellToken>()) return "BuyXTZSellToken";
		else return "";
	}

	QiupuswapTradeSide StringToQiupuswapTradeSide (const types::String& type)   {
		if      (type == "SellXTZBuyToken")return QiupuswapTradeSide{quipuswap::SellXTZBuyToken{}};
		else if (type == "BuyXTZSellToken")return QiupuswapTradeSide{quipuswap::BuyXTZSellToken{}};
		else return QiupuswapTradeSide{};
	}
  }//!namespace

  //todo: it seems that this functionality should be in the block of parsing INI file
  TradingContract::TradingContract (Ticker t)
  : ticker (t)
  , dex (trading_contract_base::TickerToDex(ticker))
  {}

  TradingContract::TradingContract (Ticker t, types::String trade_side)
		  : ticker (t)
		  , dex (trading_contract_base::TickerToDex(ticker))
		  , quipuswap_trade_side (trading_contract_base::StringToQiupuswapTradeSide(trade_side))
  {}

  bool operator < (const TradingContract& lhs, const TradingContract& rhs) {
	  using namespace trading_contract_base;
	  if (lhs.ticker == rhs.ticker) {
		  return QiupuswapTradeSideToString(lhs.quipuswap_trade_side) <
		  QiupuswapTradeSideToString(rhs.quipuswap_trade_side);
	  } else {
		  return lhs.ticker < rhs.ticker;
	  }
  }

}//!namespace