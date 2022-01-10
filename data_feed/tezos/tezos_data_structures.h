//
// Created by root on 9/16/21.
//
#pragma once

#include "types_declarations.h"
#include "timestamp.h"

#ifndef TEZOSBOT_DATA_STRUCTURES_H
#define TEZOSBOT_DATA_STRUCTURES_H

namespace algo {

  namespace tezos {

	enum class NetworkID {
		None,
		Main,
		Granada,
		Florence,
		Hangzhounet,
	};

	struct NodeAddress {
		std::string owner;
		std::string end_point;
		NetworkID network_id;
		const std::string& operator ()() {return end_point;}
	};

// from https://tezostaquito.io/docs/rpc_nodes/
	static const
	std::vector<NodeAddress>
			node_endpoint_hangzhou {{"Hangzhounet", "https://hangzhounet.api.tez.ie", NetworkID::Hangzhounet}},
			node_endpoint_granada {{"ECAD Labs nodes", "https://granadanet.api.tez.ie", NetworkID::Granada},
								   {"SmartPy nodes", "https://granadanet.smartpy.io/", NetworkID::Granada},
								   {"Nodes operated by Blockscale on behalf of the Tezos Foundation", "https://rpczero.tzbeta.net/", NetworkID::Granada}},
			node_endpoint_florence {{"ECAD Labs nodes", "https://florencenet.api.tez.ie", NetworkID::Florence}},
			node_endpoint_main {{"ECAD Labs nodes", "https://mainnet.api.tez.ie",NetworkID::Main},
								{"SmartPy nodes", "https://mainnet.smartpy.io",NetworkID::Main},
								{"Nodes operated by Blockscale on behalf of the Tezos Foundation", "https://rpc.tzbeta.net/",NetworkID::Main},
								{"LetzBake!", "https://teznode.letzbake.com", NetworkID::Main}};

  }//!namespace
}//!namespace

#endif //TEZOSBOT_DATA_STRUCTURES_H
