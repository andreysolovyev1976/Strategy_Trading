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

		  std::optional<Json::Dict> output;
		  try {
			  output =
					  std::visit(utils::overloaded{
							  []([[maybe_unused]] std::monostate arg) -> std::optional<Json::Dict> { return std::nullopt; },
							  []([[maybe_unused]] const std::string& arg) -> std::optional<Json::Dict> {
								std::stringstream ss(arg);
								auto doc = Json::Load(ss);
								auto& m = doc.GetRoot().AsDict();
								if (auto found = m.find("storage"); found==m.end()) return std::nullopt;
								else if (not found->second.IsDict()) return std::nullopt;
								else return std::optional{found->second.AsDict()};
							  },
							  [](Json::Document& arg) -> std::optional<Json::Dict> {
								auto& m = arg.GetRoot().AsDict();
								if (auto found = m.find("storage"); found==m.end()) return std::nullopt;
								else if (not found->second.IsDict()) return std::nullopt;
								else return std::optional{found->second.AsDict()};
							  }
					  }, response->body);
		  }
		  catch (std::exception& e) {
			  std::cerr << e.what();
		  }
		  return output;
	  }//!func


	}//!namespace
  }//!namespace
}//!namespace
