//
// Created by Andrey Solovyev on 24/02/2022.
//

#include "bot_config.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
//#include <boost/algorithm/string/split.hpp>
//#include <boost/algorithm/string.hpp>

namespace algo {
  namespace config {

	using types::String;
	using types::Int;
	using types::Float;
	using namespace std::literals;
	using namespace boost::property_tree;

	std::ostream& operator << (std::ostream &os, const ContractInfo& ci) {
		os
				<< '[' << ci.name << ']' << '\n'
				<< "ticker CB: " << ci.ticker_cb << '\n'
				<< "ticker QS: " << ci.ticker_qs << '\n'
				<< "price difference: " << ci.price_diff << '\n'
				<< "timeframe: " << ci.timeframe << '\n'
				<< "trade size: " << ci.trade_size << '\n'
				<< "slippage: "<< ci.slippage << '\n';

		return os;
	}

	RobotConfig& RobotConfig::getInstance(const types::String& config_file_name) {
		static RobotConfig config(config_file_name);
		return config;
	}

	RobotConfig::RobotConfig(const types::String& _) : config_file_name(_) {}

	void RobotConfig::loadFromIni(){
		try {
			ini_parser::read_ini(config_file_name, conf);
		}
		catch (const ptree_error &e) {
			throw LogicError(EXCEPTION_MSG("Error while loading: "s + config_file_name + " - " + e.what() + " "));
		}

		loadSettingsFromConfig();
	}//!func

	void RobotConfig::updateIni(ContractInfo&& contract_info) {
		ptree elem;
		elem.put("ticker_cb", contract_info.ticker_cb);
		elem.put("ticker_qs", contract_info.ticker_qs);
		elem.put("price_diff", contract_info.price_diff);
		elem.put("timeframe", contract_info.timeframe);
		elem.put("trade_size", contract_info.trade_size);
		elem.put("slippage", contract_info.slippage);

		conf.put_child(contract_info.name, std::move(elem));
		contracts_data_from_ini->operator[](contract_info.name) = std::move(contract_info);

		ini_parser::write_ini(config_file_name, conf);
	}

	const ContractInfo& RobotConfig::getContractInfo(const types::String& contract_name) const {
		if (auto ctr = contracts_data_from_ini->find(contract_name); ctr != contracts_data_from_ini->end()) {
			return contracts_data_from_ini->at(contract_name);
		}
		else {
			return EMPTY_CONTRACT;
		}
	}//!func

	void RobotConfig::printIniFile (std::ostream& os) const {
		os << "Settings loaded from .ini file:\n" << '\n';
		for (const auto& item : conf) {
			os << "[" << item.first << "]\n";
			for (const auto& value : item.second) {
				os << value.first << " = " << value.second.data() << '\n';
			}
			os << '\n';
		}
	} //!func

	void RobotConfig::printSettings (std::ostream& os) const {
		os << "Settings stored:\n" << '\n';
		os << "Tg Bot secret key: " << tg_bot_secret_key << '\n' << '\n';
		os << "Contract data\n";
		for (auto b = contracts_data_from_ini->begin(), e = contracts_data_from_ini->end(); b != e; ++b) {
			os << b->second;
		}

	}//!func

	const Contracts RobotConfig::getContracts() const {
		return contracts_by_name;
	}


	void RobotConfig::loadSettingsFromConfig() {
		try {
			tg_bot_secret_key = conf.get<String>("connection.tg_bot_id");
			for (const auto& item : conf) {
				if (item.first != "connection") {
					loadContractData(item.first);
				}
			}
		}
		catch (const ptree_error &e)
		{
			throw ConfigError(EXCEPTION_MSG("Failed to load data from configuration file - "s + e.what()));
		}
	}//!func

	void RobotConfig::loadContractData(const std::string &contract_name) {
//		_LOG(contract_name)

		auto& contract = contracts_data_from_ini->operator[](contract_name);
		contract.name = contract_name;
		loadContractDataField<String>(contract_name, "ticker_cb", contract.ticker_cb);
		loadContractDataField<String>(contract_name, "ticker_qs", contract.ticker_qs);
		loadContractDataField<Int>(contract_name, "price_diff", contract.price_diff);
		loadContractDataField<Int>(contract_name, "timeframe", contract.timeframe);
		loadContractDataField<Int>(contract_name, "trade_size", contract.trade_size);
		loadContractDataField<Int>(contract_name, "slippage", contract.slippage);

		TradingContract qs (contract_name, contract.ticker_qs);
		contracts_by_name.operator->()->insert(std::make_pair(qs.joint_name, std::move(qs)));

		TradingContract cb (contract_name, contract.ticker_cb);
		contracts_by_name.operator->()->insert(std::make_pair(cb.joint_name, std::move(cb)));
	}//!func


  }//!namespace
}//!namespace
