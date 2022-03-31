//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "object_type.h"
#include "types_declarations.h"

#ifndef STRATEGY_TRADING_TRADING_CONTRACT_H
#define STRATEGY_TRADING_TRADING_CONTRACT_H

namespace algo {

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

	}//namespace


	struct TradingContract final {
		Ticker ticker;
		trading_contract_base::Dex dex;

		explicit TradingContract (Ticker t) : ticker (t) {
			char c = checkDex ();
			 if (c == 'Q') dex.emplace<trading_contract_base::dex_source::Quipuswap>();
			 else if (c == 'C') dex.emplace<trading_contract_base::dex_source::Coinbase>();
		}
		char checkDex () const;

	};

}//!namespace

#endif //STRATEGY_TRADING_TRADING_CONTRACT_H
