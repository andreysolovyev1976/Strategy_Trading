//
// Created by root on 9/16/21.
//
#pragma once

#include "tezos_data_structures.h"


#ifndef QUIPUSWAP_DATA_STRUCTURES_H
#define QUIPUSWAP_DATA_STRUCTURES_H

namespace algo {
  namespace tezos {
	namespace quipuswap {

	  using tezos::NetworkID;

	  template <typename Duration>
	  struct TokenPair {
		  Ticker ticker;
		  types::String token_address, pair_address;
		  types::Value token_pool, tez_pool, current_price;
		  time_::Timestamp<Duration> timestamp;
	  };


	  //https://madfish.crunch.help/quipu-swap/quipu-swap-for-developers
	  // used v3 addresses
	  enum class ContractType {
		  None,
		  FA1_2,
		  FA2,
	  };
	  using Factory = std::pair<ContractType, tezos::NetworkID>;


	  struct API_version {
		  int major, minor;
	  };

	  struct FactoryAddress {
		  API_version version;
		  types::String address;
	  };

	  static const std::map <Factory, FactoryAddress> factories {
			  {Factory{ContractType::FA1_2, NetworkID::Main}, FactoryAddress{API_version{3, 0}, "KT1FWHLMk5tHbwuSsp31S4Jum4dTVmkXpfJw"}},
			  {Factory{ContractType::FA2, NetworkID::Main}, FactoryAddress{API_version{3, 0}, "KT1PvEyN1xCFCgorN92QCfYjw3axS6jawCiJ"}},
			  {Factory{ContractType::FA1_2, NetworkID::Hangzhounet}, FactoryAddress{API_version{3, 0}, "KT1HrQWkSFe7ugihjoMWwQ7p8ja9e18LdUFn"}},
			  {Factory{ContractType::FA2, NetworkID::Hangzhounet}, FactoryAddress{API_version{3, 0}, "KT1Dx3SZ6r4h2BZNQM8xri1CtsdNcAoXLGZB"}},
			  {Factory{ContractType::FA1_2, NetworkID::Granada}, FactoryAddress{API_version{3, 0}, "KT1EmfR5bSZN7mWgapE8FZKdbJ3NLjDHGZmd"}},
			  {Factory{ContractType::FA2, NetworkID::Granada}, FactoryAddress{API_version{3, 0}, "KT1SZzW5BZ6aLmcK9i3Us36angwFB67HmsYT"}},
	  };

/*
	enum class CurrencyPair {
		aave,
		binance_usd,
		celsius_network_token,
		compound,
		crypto_com_coin,
		dai,
		ftx_token,
		huobi_token,
		husd,
		leo_token,
		chainlink,
		matic_network,
		maker,
		okb,
		paxos_standard,
		sushi,
		tether,
		uniswap,
		usd_coin1,
		usd_coin2,
		wrapped_bitcoin1,
		wrapped_bitcoin2,
		weth1,
		weth2,
		tezos,
	};

	inline CurrencyPair Next(CurrencyPair pair) {
		int new_idx = static_cast<int>(pair);
		++new_idx;
		pair = static_cast<CurrencyPair>(new_idx);
		return pair;
	}

	struct CurrencyDetails {
		NetworkID tezos_network_id;
		std::string quipuswap_smart_contract_address;
		std::string coingecko_name;
	};

	static const std::unordered_map<CurrencyPair, CurrencyDetails> currencies
			{
					{CurrencyPair::aave,                  {NetworkID::Main, "KT1Lvtxpg4MiT2Bs38XGxwh3LGi5MkCENp4v", "aave"}},
					{CurrencyPair::binance_usd,           {NetworkID::Main, "KT1UMAE2PBskeQayP5f2ZbGiVYF7h8bZ2gyp", "binance-usd"}},
					{CurrencyPair::celsius_network_token, {NetworkID::Main, "KT1KuV43iebbbrkBMGov2QMgAbsnAksx6ncW", "celsius-network-token"}},
					{CurrencyPair::compound,              {NetworkID::Main, "KT1DA8NH6UqCiSZhEg5KboxosMqLghwwvmTe", "compound"}},
					{CurrencyPair::crypto_com_coin,       {NetworkID::Main, "KT1GSjkSg6MFmEMnTJSk6uyYpWXaEYFahrS4", "crypto-com-coin"}},
					{CurrencyPair::dai,                   {NetworkID::Main, "KT1PQ8TMzGMfViRq4tCMFKD2QF5zwJnY67Xn", "dai"}},
					{CurrencyPair::ftx_token,             {NetworkID::Main, "KT1SzCtZYesqXt57qHymr3Hj37zPQT47JN6x", "ftx-token"}},
					{CurrencyPair::huobi_token,           {NetworkID::Main, "KT18fp5rcTW7mbWDmzFwjLDUhs5MeJmagDSZ", "huobi-token"}},
					{CurrencyPair::husd,                  {NetworkID::Main, "KT1AN7BBmeSUN5eDDQLEhWmXv1gn4exc5k8R", "husd"}},
					{CurrencyPair::leo_token,             {NetworkID::Main, "KT1MpRQvn2VRR26VJFPYUGcB8qqxBbXgk5xe", "leo-token"}},
					{CurrencyPair::chainlink,             {NetworkID::Main, "KT1Lpysr4nzcFegC9ci9kjoqVidwoanEmJWt", "chainlink"}},
					{CurrencyPair::matic_network,         {NetworkID::Main, "KT1RsfuBee5o7GtYrdB7bzQ1M6oVgyBnxY4S", "matic-network"}},
					{CurrencyPair::maker,                 {NetworkID::Main, "KT1MaefGJRtu57DiVhQNEjYgTYok3X71iEDj", "maker"}},
					{CurrencyPair::okb,                   {NetworkID::Main, "KT1NQyNPXmjYktNBDhYkBKyTGYcJSkNbYXuh", "okb"}},
					{CurrencyPair::paxos_standard,        {NetworkID::Main, "KT1Ca5FGSeFLH3ugstc5p56gJDMPeraBcDqE", "paxos-standard"}},
					{CurrencyPair::sushi,                 {NetworkID::Main, "KT1Lotcahh85kp878JCEc1TjetZ2EgqB24vA", "sushi"}},
					{CurrencyPair::tether,                {NetworkID::Main, "KT1T4pfr6NL8dUiz8ibesjEvH2Ne3k6AuXgn", "tether"}},
					{CurrencyPair::uniswap,               {NetworkID::Main, "KT1Ti3nJT85vNn81Dy5VyNzgufkAorUoZ96q", "uniswap"}},
					{CurrencyPair::usd_coin1,             {NetworkID::Main, "KT1U2hs5eNdeCpHouAvQXGMzGFGJowbhjqmo", "usd-coin"}},
					{CurrencyPair::usd_coin2,             {NetworkID::Main, "KT1WxgZ1ZSfMgmsSDDcUn8Xn577HwnQ7e1Lb", "usd-coin"}},
					{CurrencyPair::wrapped_bitcoin1,      {NetworkID::Main, "KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1", "wrapped-bitcoin"}},
					{CurrencyPair::wrapped_bitcoin2,      {NetworkID::Main, "KT1WBLrLE2vG8SedBqiSJFm4VVAZZBytJYHc", "wrapped-bitcoin"}},
					{CurrencyPair::weth1,                 {NetworkID::Main, "KT1DuYujxrmgepwSDHtADthhKBje9BosUs1w", "weth"}},
					{CurrencyPair::weth2,                 {NetworkID::Main, "KT1Evsp2yA19Whm24khvFPcwimK6UaAJu8Zo", "weth"}},
					{CurrencyPair::tezos,                 {NetworkID::Main, "",                                     "tezos"}},
			};
*/
	}//!namespace
  }//!namespace
}//!namespace

#endif //QUIPUSWAP_DATA_STRUCTURES_H
