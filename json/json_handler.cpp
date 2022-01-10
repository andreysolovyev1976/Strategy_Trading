//
// Created by Andrey Solovyev on 06/12/2021.
//

#include "json_handler.h"
#include "exceptions.h"
#include "curl_client/utils.h"

namespace Json {


  std::vector<const Json::Node*> Search::findValue(const Json::Node &node, const types::String &string_to_look_for)  {
	  found.clear();
	  findValueImpl(node, string_to_look_for);
	  return found;
  }


  void Search::findValueImpl(const Json::Node &node, const types::String &string_to_look_for)  {
	  std::visit(utils::overloaded {
			  []([[maybe_unused]] std::nullptr_t arg) {},
			  [&string_to_look_for, this](Array arg) {findStrInArray(arg, string_to_look_for);},
			  [&string_to_look_for, this](Dict arg) {findStrInDictionary(arg, string_to_look_for);},
			  []([[maybe_unused]] bool arg) {},
			  []([[maybe_unused]] BigInt arg) {},
			  []([[maybe_unused]] Float arg) {},
			  [&string_to_look_for, this](const types::String &arg) {findStrInValue(arg, string_to_look_for);},
	  }, node.GetValue());
  }

  void Search::findStrInArray (const Json::Node &node, const types::String &string_to_look_for)  {
	  for (const auto &elem : node.AsArray()) {
		  findValueImpl(elem, string_to_look_for);
	  }
  }

  void Search::findStrInDictionary(const Json::Node &node, const types::String &string_to_look_for)  {
	  //this func is effectively a dfs
	  //search for the value in the keys
	  const auto &dict = node.AsDict();
	  auto it = dict.find(string_to_look_for);
	  if (it != dict.end()) found.emplace_back(&it->second);

	  //search for the string_value in the values, is they are maps or arrays
	  for (const auto &[key, value]: dict) {
		  if (value.IsDict() || value.IsArray()) {
			  findValueImpl(value, string_to_look_for);
		  }
	  }
  }

  void Search::findStrInValue (const Json::Node &node, const types::String &string_to_look_for)  {
	  if (node.IsString() && node.AsString() == string_to_look_for)  found.emplace_back(&node);
  }

}//!namespace