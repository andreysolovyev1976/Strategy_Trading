//
// Created by Andrey Solovyev on 22/03/2022.
//

#include "dex_transaction.h"
#include "user_defined_exceptions.h"
#include "os_pipe.h"
#include "const_values.h"
#include "tezos_data_structures.h"

namespace algo {
  namespace tezos {
	namespace quipuswap {
	  namespace transaction {

		types::String makeCommand (const Trade& trade, const types::String& key) {
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
			}
			else if (auto p2 =
						t_contract.quipuswap_trade_side.TryAs<trading_contract_base::quipuswap::BuyXTZSellToken>();
					p2) {
				command += "from_token_to_tez.ts";
			}
			else {
				throw InvalidArgumentError(EXCEPTION_MSG("Trying to trade with undefined trade side - either from Tez or to Tez; "));
			}

			command += " --someAsset=";
			command += t_contract.ticker;
			command += " --inputValue=";
			command += trade.getQuantity().ToString();
			command += " --slippageTolerance=";
			command += trade.getSlippage().ToString();
			command += " --pk=";
			command += key;

			std::cerr << command << '\n';

			return command;
		}//!func

	  }//!namespace
	}//!namespace
  }//!namespace
}//!namespace
