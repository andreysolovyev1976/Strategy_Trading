//
// Created by Andrey Solovyev on 04/04/2022.
//

#pragma once

#include "user_defined_strings.h"
#include "user_defined_exceptions.h"
#include "base_objects_container.h"

#ifndef STRATEGY_TRADING_MODIFIER_FUNCS_H
#define STRATEGY_TRADING_MODIFIER_FUNCS_H

namespace algo {

  template <typename T>
  class Modifier{
  public:
	  explicit Modifier(const types::String& l) : label (l) {}
	  void setModifierValue(T t) {modifier_value = t;}

	  const types::String& getLabel () const {return label;}

	  std::function<T(const T&)> getModifierMethod (const types::String& ui_input) {
		  auto value = modifier_value;
		  if (ui_input == "add") {
			  std::function<T(const T&)> M([value](const T& t) {return t + value;});
			  return M;
		  }
		  else if (ui_input == "subtract") {
			  std::function<T(const T&)> M([value](const T& t) {return t - value;});
			  return M;
		  }
		  else if (ui_input == "multiply") {
			  std::function<T(const T&)> M([value](const T& t) {return t * value;});
			  return M;
		  }
		  else if (ui_input == "divide") {
			  std::function<T(const T&)> M([value](const T& t) {return t / value;});
			  return M;
		  }
		  else throw RuntimeError(EXCEPTION_MSG("Can't find a method at Modifier class; "));
	  }

  private:
	  T modifier_value;
	  types::String label;
  };


  template <typename QuoteType>
  class Modifiers : public Objects<Modifier<QuoteType>> {
  public:
	  using Objects<Modifier<QuoteType>>::Objects;

	  //todo: it is unclear why I have here a compile error
//	  ByTicker& getByTicker () = delete;
//	  const ByTicker& getByTicker () const = delete;

	  void updateModifier (const types::String& label, const QuoteType& quote) {
		  if (auto found = this->getByLabel()->find(label); found == this->getByLabel()->end()){
			  throw InvalidArgumentError(EXCEPTION_MSG("No Modifier found; "));
		  } else {
			  found->second->setModifierValue(quote);
		  }
	  }

	  void addModifier (Modifier<QuoteType> modifier) {
		  auto label = modifier.getLabel();
		  if (auto found = this->getByLabel()->find(label); found != this->getByLabel()->end()){
			  throw InvalidArgumentError(EXCEPTION_MSG("Modifier already exists with this Label; "));
		  }
		  auto new_modifier_ptr = Ptr<Modifier<QuoteType>>(std::move(modifier));
		  this->getByLabel()->insert({new_modifier_ptr->getLabel(), std::move(new_modifier_ptr)});
	  }
  };

}//!namespace


#endif //STRATEGY_TRADING_MODIFIER_FUNCS_H
