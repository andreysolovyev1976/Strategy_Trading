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
#if defined(__APPLE__)
			command += "ts-node ../setup_transacton_ts_script/quipuswap_transaction.ts";
#endif
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
			command += "systemctl ts-node ../setup_transacton_ts_script/quipuswap_transaction.ts";
#endif

			if (auto p1 =
						t_contract.quipuswap_trade_side.TryAs<trading_contract_base::quipuswap::SellXTZBuyToken>();
					p1) {
				command += " --TradeSide=fromTez";
				command += " --Asset=";
				command += t_contract.ticker;
			}
			else if (auto p2 =
						t_contract.quipuswap_trade_side.TryAs<trading_contract_base::quipuswap::BuyXTZSellToken>();
					p2) {

				command += " --TradeSide=toTez";
				command += " --Asset=";
				command += t_contract.ticker;
			}
			else {
				throw InvalidArgumentError(EXCEPTION_MSG("Trying to trade with undefined trade side - either from Tez or to Tez; "));
			}

			command += " --inputValue=";
			command += trade.getQuantity().ToString();
			command += " --slippageTolerance=";
			command += trade.getSlippage().ToString();

			std::cerr << command << '\n';

			return command;
		}//!func

	  }//!namespace
	}//!namespace
  }//!namespace
}//!namespace
