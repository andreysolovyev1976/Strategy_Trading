//
// Created by Andrey Solovyev on 14/12/2021.
//

#pragma once

#include "quipuswap_data_structures.h"
#include "user_defined_exceptions.h"
#include "tzkt_io.h"
#include "json.h"
#include "curl_client.h"
#include "utils.h"

#include <optional>

#ifndef STRATEGY_TRADING_QUIPUSWAP_PRICE_EXPECTATION_H
#define STRATEGY_TRADING_QUIPUSWAP_PRICE_EXPECTATION_H

/*

 const factories = {
    fa1_2Factory: "KT1Lw8hCoaBrHeTeMXbqHPG4sS4K1xn7yKcD",
    fa2Factory: "KT1SwH9P1Tx8a58Mm6qBExQFTcy2rwZyZiXS",
};

const getQuipuExchangeRate = async (token: Token, retryCount?: number): Promise<string> => {
    try {
        const dex = await findDex(tezos, factories, token);
        const dexStorage: any = await dex.contract.storage();

        const tezValue = 1000_000;

        return estimateTezToToken(dexStorage, tezValue).toString();

    } catch (err: any) {
        let error: any = JSON.parse(JSON.stringify(err))
        message = `Encountered error: ${error}\n---`
        console.log(message, err);

        retryCount = retryCount ? retryCount : 0
        if (retryCount < RETRY_MAX_COUNT) {
            retryCount++
            console.log(`Experienced an error while fetching quipuswap exchange rate, retrying ${retryCount}`);

            return await getQuipuExchangeRate(token, retryCount)
        }

        return '0'
    }
}



 import BigNumber from "bignumber.js";
import { assertNat } from "./helpers";
import { FEE_FACTOR } from "./defaults";

export function estimateTezToToken(
  dexStorage: any,
  tezValue: BigNumber.Value
): BigNumber {
  const tezValueBN = new BigNumber(tezValue);
  assertNat(tezValueBN);
  if (tezValueBN.isZero()) return new BigNumber(0);

  const tezInWithFee = new BigNumber(tezValue).times(FEE_FACTOR);
  const numerator = tezInWithFee.times(dexStorage.storage.token_pool);
  const denominator = new BigNumber(dexStorage.storage.tez_pool)
    .times(1000)
    .plus(tezInWithFee);
  return numerator.idiv(denominator);
}

export function estimateTokenToTez(
  dexStorage: any,
  tokenValue: BigNumber.Value
) {
  const tokenValueBN = new BigNumber(tokenValue);
  assertNat(tokenValueBN);
  if (tokenValueBN.isZero()) return new BigNumber(0);

  const tokenInWithFee = new BigNumber(tokenValue).times(FEE_FACTOR);
  const numerator = tokenInWithFee.times(dexStorage.storage.tez_pool);
  const denominator = new BigNumber(dexStorage.storage.token_pool)
    .times(1000)
    .plus(tokenInWithFee);
  return numerator.idiv(denominator);
}

export function estimateTezToTokenInverse(
  dexStorage: any,
  tokenValue: BigNumber.Value
) {
  const tokenValueBN = new BigNumber(tokenValue);
  assertNat(tokenValueBN);
  if (tokenValueBN.isZero()) return new BigNumber(0);

  const numerator = new BigNumber(dexStorage.storage.tez_pool)
    .times(1000)
    .times(tokenValue);
  const denominator = new BigNumber(dexStorage.storage.token_pool)
    .minus(tokenValue)
    .times(FEE_FACTOR);
  return numerator.idiv(denominator).plus(1);
}

export function estimateTokenToTezInverse(
  dexStorage: any,
  tezValue: BigNumber.Value
) {
  const tezValueBN = new BigNumber(tezValue);
  assertNat(tezValueBN);
  if (tezValueBN.isZero()) return new BigNumber(0);

  const numerator = new BigNumber(dexStorage.storage.token_pool)
    .times(1000)
    .times(tezValue);
  const denominator = new BigNumber(dexStorage.storage.tez_pool)
    .minus(tezValue)
    .times(FEE_FACTOR);
  return numerator.idiv(denominator).plus(1);
}

export function estimateSharesInTez(
  dexStorage: any,
  tezValue: BigNumber.Value
) {
  const tezValueBN = new BigNumber(tezValue);
  assertNat(tezValueBN);
  if (tezValueBN.isZero()) return new BigNumber(0);

  return tezValueBN
    .times(dexStorage.storage.total_supply)
    .idiv(dexStorage.storage.tez_pool);
}

export function estimateSharesInToken(
  dexStorage: any,
  tokenValue: BigNumber.Value
) {
  const tokenValueBN = new BigNumber(tokenValue);
  assertNat(tokenValueBN);
  if (tokenValueBN.isZero()) return new BigNumber(0);

  return tokenValueBN
    .times(dexStorage.storage.total_supply)
    .idiv(dexStorage.storage.token_pool);
}

export function estimateTezInShares(dexStorage: any, shares: BigNumber.Value) {
  const sharesBN = new BigNumber(shares);
  assertNat(sharesBN);
  if (sharesBN.isZero()) return new BigNumber(0);

  return sharesBN
    .times(dexStorage.storage.tez_pool)
    .idiv(dexStorage.storage.total_supply);
}

export function estimateTokenInShares(
  dexStorage: any,
  shares: BigNumber.Value
) {
  const sharesBN = new BigNumber(shares);
  assertNat(sharesBN);
  if (sharesBN.isZero()) return new BigNumber(0);

  return sharesBN
    .times(dexStorage.storage.token_pool)
    .idiv(dexStorage.storage.total_supply);
}

export function estimateTokenInTez(dexStorage: any, tezValue: BigNumber.Value) {
  const tezValueBN = new BigNumber(tezValue);
  assertNat(tezValueBN);
  if (tezValueBN.isZero()) return new BigNumber(0);

  return tezValueBN
    .times(dexStorage.storage.token_pool)
    .idiv(dexStorage.storage.tez_pool);
}

export function estimateTezInToken(
  dexStorage: any,
  tokenValue: BigNumber.Value
) {
  const tokenValueBN = new BigNumber(tokenValue);
  assertNat(tokenValueBN);
  if (tokenValueBN.isZero()) return new BigNumber(0);

  return tokenValueBN
    .times(dexStorage.storage.tez_pool)
    .idiv(dexStorage.storage.token_pool);
}

export function isDexContainsLiquidity(dexStorage: any) {
  return !(
    new BigNumber(dexStorage.storage.tez_pool).isZero() ||
    new BigNumber(dexStorage.storage.token_pool).isZero()
  );
}

export function withSlippage(val: BigNumber.Value, tolerance: BigNumber.Value) {
  return new BigNumber(val)
    .times(new BigNumber(1).minus(tolerance))
    .integerValue(BigNumber.ROUND_DOWN);
}
 */

namespace algo {
  namespace tezos {
	namespace quipuswap {

	  template <typename Number>
	  constexpr bool assertNat(Number i) {
		  if constexpr (not types::IsInteger<Number>(i) || types::IsNegative(i)) {
			  throw std::runtime_error(EXCEPTION_MSG("Value is not non-negative natural number; "));
		  }
		  return true;
	  }

	  std::optional<Json::Dict> getStorage (const types::String &contract_address,
			  curl_client::Response& response, curl_client::Request& request
	  );

	  inline types::Value fromJsonToValue (const Json::Node &node) {
		  if (node.IsInt()) return types::Value (node.AsInt());
		  else if (node.IsDouble()) return types::Value (node.AsDouble());
		  else if (node.IsString()) return types::Value (node.AsString());
		  else throw std::invalid_argument (EXCEPTION_MSG("Wrong argument to create a Value from; "));
	  }

	  template <typename Duration>
	  TokenPair<Duration> updateToken (TokenPair<Duration> token,
			  curl_client::Response& response, curl_client::Request& request
	  ) {
		  if (token.pair_address.empty()) throw std::invalid_argument(EXCEPTION_MSG("Token address is empty; "));

		  auto storage = getStorage(token.pair_address, response, request);
		  if (not storage.has_value()) throw std::runtime_error(EXCEPTION_MSG("Bad response from tzkt; "));
		  token.timestamp = time_::Timestamp<Duration>();

		  auto token_pool = storage->find("token_pool");
		  auto tez_pool = storage->find("tez_pool");
		  if (token_pool == storage->end() || tez_pool == storage->end()) {
			  throw std::runtime_error(EXCEPTION_MSG("Corrupted dictionary from tzkt; "));
		  }
		  token.token_pool = fromJsonToValue (token_pool->second);
		  token.tez_pool = fromJsonToValue (tez_pool->second);

		  return token;
	  }

	  template <typename Duration>
	  TokenPair<Duration> estimateTezToToken(TokenPair<Duration> token,
			  curl_client::Response& response, curl_client::Request& request) {
		  token = updateToken(token, response, request);
		  token.current_price_tez =
		  		const_values::FEE_VALUE_PRODUCT * token.token_pool
				  / (token.tez_pool + const_values::FEE_VALUE_PRODUCT)
				  / const_values::VALUE_FOR_QUOTE_DETERMINATION; token.current_price_tez /= 100;
		  return std::move(token); //got compiler warning on copying attempt
	  }

	  template <typename Duration>
	  TokenPair<Duration> estimateTokenToTez(TokenPair<Duration> token,
			  curl_client::Response& response, curl_client::Request& request) {
		  token = updateToken(token, response, request);
		  token.current_price_token =
		  		const_values::FEE_VALUE_PRODUCT * token.tez_pool
				  / (token.token_pool + const_values::FEE_VALUE_PRODUCT)
		  		  / const_values::VALUE_FOR_QUOTE_DETERMINATION;
		  return std::move(token); //got compiler warning on copying attempt
	  }

	}//!namespace
  }//!namespace
}//!namespace

#endif //STRATEGY_TRADING_QUIPUSWAP_PRICE_EXPECTATION_H
