//
// Created by Andrey Solovyev on 06/12/2021.
//

#pragma once

#include "json.h"
#include "types_declarations.h"

#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

namespace Json {
  
  class Search final {
  public:
  	std::vector<const Json::Node*> findValue(const Json::Node &node, const types::String &string_to_look_for);
  private:
	  std::vector<const Json::Node*> found;

	  void findValueImpl (const Json::Node &v, const types::String &string_to_look_for);
	  void findStrInDictionary(const Json::Node &node, const types::String &string_to_look_for);
	  void findStrInArray (const Json::Node &node, const types::String &string_to_look_for);
	  void findStrInValue (const Json::Node &node, const types::String &string_to_look_for);
  };
}//!namespace
#endif //JSON_HANDLER_H