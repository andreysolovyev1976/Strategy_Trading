//
// Created by Andrey Solovyev on 24/02/2022.
//

#pragma once

#include "types_declarations.h"
#include "user_defined_exceptions.h"
#include "safe_ptr.h"
#include "const_values.h"
#include "maps.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include <map>
#include <vector>
#include <ostream>

#ifndef STRATEGY_TRADING_BOT_CONFIG_H
#define STRATEGY_TRADING_BOT_CONFIG_H

namespace algo {
  namespace config {

	struct ContractInfo {
		types::String
				name = const_values::NO_NAME, 	// user's given contract name
				ticker_cb, 	 					// ticker1
				ticker_qs;  					// ticker2
		types::Int
				price_diff = const_values::PRICE_DIFF,
				timeframe  = const_values::TIMEFRAME,
				trade_size = const_values::TRADE_SIZE,
				slippage   = const_values::SLIPPAGE;
	};
	const static ContractInfo EMPTY_CONTRACT;
	std::ostream& operator << (std::ostream &os, const ContractInfo& ci);

	struct TradingContract {
		types::String name;
		Ticker ticker;
		types::String joint_name;
		explicit TradingContract (const types::String& name_, const Ticker& ticker_)
				: name(name_)
				, ticker(ticker_)
				, joint_name(types::concatenateStrings('.', name, ticker))
		{}
	};

	using Contracts = types::MultiThreadedMap<types::String, TradingContract>;

	class RobotConfig final {
	public:
		/// making sure there is the only instance, works along with private ctors
		static RobotConfig& getInstance(const types::String& config_file_name);

		void loadFromIni();
		void updateIni(ContractInfo&& contract_info);
		const ContractInfo& getContractInfo(const types::String& contract_name) const;

		void printIniFile (std::ostream& os) const;
		void printSettings (std::ostream& os) const;

		const Contracts& getContracts() const;

	private:
		boost::property_tree::ptree conf;
		safe_ptr<std::map<types::String, ContractInfo>> contracts_data_from_ini;
		Contracts contracts_by_name;

		types::String tg_bot_secret_key;
		const types::String config_file_name;

		RobotConfig(const types::String& _);
		RobotConfig(RobotConfig const&) = delete;
		void operator = (RobotConfig const&) = delete;

		void loadSettingsFromConfig();
		void loadContractData(const types::String& contract_name);
		template <typename T>
		void loadContractDataField(const types::String &contract_name, const types::String& field_name, T& value);

	};//!class

	template <typename T>
	void RobotConfig::loadContractDataField(const types::String &contract_name, const types::String& field_name, T& value) {
		types::String full_field_name;
		full_field_name.reserve(contract_name.size() + field_name.size() + 1);
		full_field_name = types::concatenateStrings('.', contract_name, field_name);

		if (auto o = conf.get_optional<T>(full_field_name); o) {
			value = std::move(o.value());
		}
	}//!func


  }//!namespace
}//!namespace

#endif //STRATEGY_TRADING_BOT_CONFIG_H


/*
 back up copy of a sample ini file
[connection]
# Parameters for connecting to IB Gateway
tg_bot_id = localhost

[name1]
ticker_cb = "ETC"
ticker_qs = "BTC"
price_diff = 12
timeframe = 60
trade_size = 100
slippage = 3


 */