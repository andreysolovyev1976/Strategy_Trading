//
// Created by Andrey Solovyev on 22/03/2022.
//

#include "dex_transaction.h"

namespace algo {
  namespace tezos {
	namespace quipuswap {
	  namespace transaction {

		types::String makeCommand (const Trade& trade) {
			const auto& t_contract = trade.getTradingContract();
			if (auto dex = t_contract.dex.TryAs<trading_contract_base::dex_source::Quipuswap>(); not dex) {
				throw InvalidArgumentError(EXCEPTION_MSG("Trying to trade with Coinbase contract at Quipuswap; "));
			}

			types::String command;
			command += const_values::TS_NODE_COMMAND;
			if (auto p1 =
						t_contract.quipuswap_trade_side.TryAs<trading_contract_base::quipuswap::SellXTZBuyToken>();
					p1) {
				command += "from_tez_to_token.ts";
				command += " --someAsset=";
				command += t_contract.ticker;
			}
			else if (auto p2 =
						t_contract.quipuswap_trade_side.TryAs<trading_contract_base::quipuswap::BuyXTZSellToken>();
					p2) {

				command += "from_token_to_tez.ts";
				command += " --someAsset=";
				command += t_contract.ticker;
			}
			else {
				throw InvalidArgumentError(EXCEPTION_MSG("Trying to trade with undefined trade side - either from Tez or to Tez; "));
			}

			command += " --inputValue=";
			command += trade.getQuantity().ToString();
			command += " --slippageTolerance=";
			command += trade.getSlippage().ToString();

//			std::cerr << command << '\n';

			return command;
		}//!func

	  }//!namespace
	}//!namespace
  }//!namespace
}//!namespace
