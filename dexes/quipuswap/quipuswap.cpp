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
#if defined(__APPLE__)
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

#endif
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)

		  if (std::holds_alternative<std::monostate>(response->body)) {return std::nullopt;}
		  else if (std::holds_alternative<std::string>(response->body)) {return std::nullopt;}
		  else if (std::holds_alternative<Json::Document>(response->body)) {
			  const auto& r = std::get<Json::Document>(response->body).GetRoot().AsDict();
			  if (auto found = r.find("storage"); found == r.end()) return std::nullopt;
			  else if (not found->second.IsDict()) return std::nullopt;
			  else return std::optional{found->second.AsDict()};
		  }
		  else {
		  	return std::nullopt;
		  }
#endif
	  }//!func


	}//!namespace
  }//!namespace
}//!namespace
