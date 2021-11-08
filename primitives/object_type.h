//
// Created by Andrey Solovyev on 03/11/2021.
//

#pragma once

#include <variant>

#ifndef STRATEGY_TRADING_OBJECT_TYPE_H
#define STRATEGY_TRADING_OBJECT_TYPE_H

namespace types {

  template <typename Variant>
  struct ObjectType : Variant {
	  using Variant::Variant;

	  ObjectType(Variant other_variant) :Variant(std::move(other_variant)) {}

	  template <typename T>
	  bool Is() const {
		  return std::holds_alternative<T>(*this);
	  }

	  template <typename T>
	  const T& As() const {
		  return std::get<T>(*this);
	  }

	  template <typename T>
	  const T* TryAs() const {
		  return std::get_if<T>(this);
	  }

	  const Variant& GetValue() const {return *this;}
	  Variant& GetValue() {return *this;}

  };

}//!namespace

#endif //STRATEGY_TRADING_OBJECT_TYPE_H
