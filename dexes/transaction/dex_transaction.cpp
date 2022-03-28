//
// Created by Andrey Solovyev on 22/03/2022.
//

#include "dex_transaction.h"

namespace algo {
  namespace tezos {
	namespace quipuswap {
	  namespace transaction {

		types::String makeCommand (const types::String& contract_address) {
			types::String command;
#if defined(__APPLE__)
			command += "ts-node ../setup_transacton_ts_script/test_transaction.ts --sourcePath=";
#endif
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
			command += "systemctl ts-node ../setup_transacton_ts_script/test_transaction.ts --sourcePath=";
#endif

			command += contract_address;
			command += " --targetPath=\"passed freaking Value\"";
			return command;
		}//!func


	  }
	}
  }
}
