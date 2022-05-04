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
				<< "ticker QS: " << ci.ticker_qs << '\n';

		return os;
	}

	RobotConfig& RobotConfig::getInstance(const types::String& config_file_name) {
		static RobotConfig config(config_file_name);
		return config;
	}

	RobotConfig::RobotConfig(const types::String& _) : config_file_name(_) {}

	void RobotConfig::loadFromIni(){
		try {
  			ini_parser::read_ini(config_file_name, configuration);
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
		elem.put("timeframe", contract_info.timeframe);

		configuration.put_child(contract_info.name, std::move(elem));
		contracts_data_from_ini->operator[](contract_info.name) = std::move(contract_info);

		ini_parser::write_ini(config_file_name, configuration);
	}

	void RobotConfig::updateIni(const types::String& new_key) {
		tpk = new_key;
		ptree elem;
		elem.put("pk", tpk);
		configuration.put_child("tezos", std::move(elem));

		ini_parser::write_ini(config_file_name, configuration);
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
		for (const auto& item : configuration) {
			os << "[" << item.first << "]\n";
			for (const auto& value : item.second) {
				os << value.first << " = " << value.second.data() << '\n';
			}
			os << '\n';
		}
	} //!func

	void RobotConfig::printSettings (std::ostream& os) const {
		printKey(os);
		printContracts(os);
	}//!func
	void RobotConfig::printKey (std::ostream& os) const {
		os << "Settings stored:\n" << '\n';
		os << "Tezos key: " << tpk << '\n' << '\n';
	}//!func

	void RobotConfig::printContracts (std::ostream& os) const {
		os << "Contract data\n";
		for (auto b = contracts_data_from_ini->begin(), e = contracts_data_from_ini->end(); b != e; ++b) {
			os << b->second << '\n';
		}
	}//!func


	const Contracts RobotConfig::getContracts() const {
		return contracts_by_name;
	}
	const types::String& RobotConfig::getKey() const {
		return tpk;
	}
	void RobotConfig::loadSettingsFromConfig() {
		try {
			tpk = configuration.get<String>("tezos.pk");
			for (const auto& item : configuration) {
				if (item.first != "tezos") {
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
		loadContractDataField<Int>(contract_name, "timeframe", contract.timeframe);

		TradingContract qs (contract_name, contract.ticker_qs);
		contracts_by_name.operator->()->insert(std::make_pair(qs.joint_name, std::move(qs)));

		TradingContract cb (contract_name, contract.ticker_cb);
		contracts_by_name.operator->()->insert(std::make_pair(cb.joint_name, std::move(cb)));
	}//!func


  }//!namespace
}//!namespace
