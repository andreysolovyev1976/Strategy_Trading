//
// Created by Andrey Solovyev on 22/03/2022.
//

#include "dex_transaction.h"
#include <cstdlib>

namespace algo {
  namespace tezos {
	namespace quipuswap {
	  namespace transaction {


		void makeTransaction([[maybe_unused]] const types::String& contract_address){

#if defined(__APPLE__)
	system ("ts-node ../setup_transacton_ts_script/test_transaction.ts --sourcePath=passedValue --targetPath=\"passed freaking Value\"");
#endif
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
	system ("systemctl ts-node ../setup_transacton_ts_script/test_transaction.ts --sourcePath=passedValue --targetPath=\"passed freaking Value\"");
#endif
}//!func


	  }
	}
  }
}
