//
// Created by Andrey Solovyev on 14/12/2021.
//

#include "quipuswap.h"


namespace algo {
  namespace tezos {
	namespace quipuswap {

	  std::optional<Json::Dict> getStorage (const types::String &contract_address) {
		  using namespace algo::tezos::tzkt_io;
		  QueryFactory qf;
		  curl_client::Response response;
		  curl_client::Request request;
		  auto handle = qf.getStorageQuery(contract_address);
		  response = request.
									pathSetNew(std::move(handle))->
									Implement(curl_client::Method::Get);

		  return
				  std::visit(utils::overloaded {
						  []([[maybe_unused]] std::monostate arg) -> std::optional<Json::Dict> { return std::nullopt; },
						  []([[maybe_unused]] const std::string &arg) -> std::optional<Json::Dict> { return std::nullopt; },
						  [](const Json::Document& arg) -> std::optional<Json::Dict> {
							const auto &m = arg.GetRoot().AsDict();
							if (auto found = m.find("storage"); found == m.end()) return std::nullopt;
							else if (not found->second.IsDict()) return std::nullopt;
							else return std::optional{found->second.AsDict()};
						  }
				  }, response->body);
	  }


	}//!namespace
  }//!namespace
}//!namespace
