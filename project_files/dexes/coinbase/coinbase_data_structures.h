//
// Created by Andrey Solovyev on 15/12/2021.
//

#pragma once

#include "types_declarations.h"
#include <unordered_map>

#ifndef STRATEGY_TRADING_COINBASE_DATA_STRUCTURES_H
#define STRATEGY_TRADING_COINBASE_DATA_STRUCTURES_H

namespace algo {
  namespace data {
	namespace coinbase {

	  static const auto input_fmt = []() -> types::String { return "%Y-%m-%dT%H:%M:%S"; }();

	  struct CurrencyData {
		  bool empty = true;
		  types::String
				  crypto_address_link,
				  crypto_transaction_link,
				  display_name,
				  type,
				  id,
				  max_precision,
				  min_size,
				  name,
				  status;
		  types::BigInt
				  max_withdrawal_amount,
				  min_withdrawal_amount;

	  };


	  struct TradeData {
		  bool empty = true;
		  types::String
				  ask,
				  bid,
				  price,
				  size,
				  time,
				  volume;
		  types::BigInt trade_id;
	  };

	  namespace error {
		enum class Code {
			two_factor_required,
			param_required,
			validation_error,
			invalid_request,
			personal_details_required,
			identity_verification_required,
			jumio_verification_required,
			jumio_face_match_verification_required,
			unverified_email,
			authentication_error,
			invalid_token,
			revoked_token,
			expired_token,
			invalid_scope,
			not_found,
			rate_limit_exceeded,
			internal_server_error,
		};

		static const std::unordered_map<Code, types::String> code_to_string{
				{Code::two_factor_required, "two_factor_required"},
				{Code::param_required, "param_required"},
				{Code::validation_error, "validation_error"},
				{Code::invalid_request, "invalid_request"},
				{Code::personal_details_required, "personal_details_required"},
				{Code::identity_verification_required, "identity_verification_required"},
				{Code::jumio_verification_required, "jumio_verification_required"},
				{Code::jumio_face_match_verification_required, "jumio_face_match_verification_required"},
				{Code::unverified_email, "unverified_email"},
				{Code::authentication_error, "authentication_error"},
				{Code::invalid_token, "invalid_token"},
				{Code::revoked_token, "revoked_token"},
				{Code::expired_token, "expired_token"},
				{Code::invalid_scope, "invalid_scope"},
				{Code::not_found, "not_found"},
				{Code::rate_limit_exceeded, "rate_limit_exceeded"},
				{Code::internal_server_error, "internal_server_error"},
		};
		static const std::unordered_map<types::String, Code> string_to_code{
				{"two_factor_required", Code::two_factor_required},
				{"param_required", Code::param_required},
				{"validation_error", Code::validation_error},
				{"invalid_request", Code::invalid_request},
				{"personal_details_required", Code::personal_details_required},
				{"identity_verification_required", Code::identity_verification_required},
				{"jumio_verification_required", Code::jumio_verification_required},
				{"jumio_face_match_verification_required", Code::jumio_face_match_verification_required},
				{"unverified_email", Code::unverified_email},
				{"authentication_error", Code::authentication_error},
				{"invalid_token", Code::invalid_token},
				{"revoked_token", Code::revoked_token},
				{"expired_token", Code::expired_token},
				{"invalid_scope", Code::invalid_scope},
				{"not_found", Code::not_found},
				{"rate_limit_exceeded", Code::rate_limit_exceeded},
				{"internal_server_error", Code::internal_server_error},
		};

		static const std::unordered_map<Code, types::String> code_description{
				{Code::two_factor_required, R"("When sending money over 2fa limit")"},
				{Code::param_required, R"("Missing parameter")"},
				{Code::validation_error, R"("Unable to validate POST/PUT")"},
				{Code::invalid_request, R"("Invalid request")"},
				{Code::personal_details_required, R"("User's personal detail required to complete this request")"},
				{Code::identity_verification_required, R"("Identity verification is required to complete this request")"},
				{Code::jumio_verification_required, R"("Document verification is required to complete this request")"},
				{Code::jumio_face_match_verification_required, R"("Document verification including face match is required to complete this request")"},
				{Code::unverified_email, R"("User has not verified their email")"},
				{Code::authentication_error, R"("Invalid auth generic")"},
				{Code::invalid_token, R"("Invalid Oauth token")"},
				{Code::revoked_token, R"("Revoked Oauth token")"},
				{Code::expired_token, R"("Expired Oauth token")"},
				{Code::invalid_scope, R"("User hasn't authenticated necessary scope")"},
				{Code::not_found, R"("Resource not found")"},
				{Code::rate_limit_exceeded, R"("Rate limit exceeded")"},
				{Code::internal_server_error, R"("Internal server error")"},
		};

		static const std::unordered_map<Code, int> code_numeric_responses{
				{Code::two_factor_required, 402},
				{Code::param_required, 400},
				{Code::validation_error, 400},
				{Code::invalid_request, 400},
				{Code::personal_details_required, 400},
				{Code::identity_verification_required, 400},
				{Code::jumio_verification_required, 400},
				{Code::jumio_face_match_verification_required, 400},
				{Code::unverified_email, 400},
				{Code::authentication_error, 401},
				{Code::invalid_token, 401},
				{Code::revoked_token, 401},
				{Code::expired_token, 401},
				{Code::invalid_scope, 403},
				{Code::not_found, 404},
				{Code::rate_limit_exceeded, 429},
				{Code::internal_server_error, 500},
		};

	  }//!namespace
	}//!namespace
  }//!namespace
}//!namespace



#endif //STRATEGY_TRADING_COINBASE_DATA_STRUCTURES_H
