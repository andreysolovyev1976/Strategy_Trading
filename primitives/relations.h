//
// Created by Andrey Solovyev on 03/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "operators.h"
#include "object_type.h"
#include "exceptions.h"

#include <type_traits>
#include <array>
#include <string>

#ifndef STRATEGY_TRADING_RELATIONS_H
#define STRATEGY_TRADING_RELATIONS_H

namespace relations {

  /// equality
  template<typename L,typename R>
  using HasEQ = std::enable_if_t <operators::has_equality<L, R>::value, bool>;

  template<typename L,typename R, HasEQ<L, R> = true>
  constexpr auto EQ = [](const L &l, const R &r) ->bool {return l == r;};

  /// inequality
  template<typename L,typename R>
  using HasNE = std::enable_if_t <operators::has_inequality<L, R>::value, bool>;

  template<typename L,typename R, HasNE<L, R> = true>
  constexpr auto NE = [](const L &l, const R &r) ->bool {return l != r;};


  /// less than
  template<typename L,typename R>
  using HasLT = std::enable_if_t <operators::has_less_than<L, R>::value, bool>;

  template<typename L, typename R, HasLT<L, R> = true>
  constexpr auto LT = [](const L &l, const R &r) ->bool {return l < r;};


  /// less or equal
  template<typename L,typename R>
  using HasLE = std::enable_if_t <operators::has_less_equal<L, R>::value, bool>;

  template<typename L,typename R, HasLE<L, R> = true>
  constexpr auto LE = [](const L &l, const R &r) ->bool {return l <= r;};


  /// greater than
  template<typename L,typename R>
  using HasGT = std::enable_if_t <operators::has_greater_than<L, R>::value, bool>;

  template<typename L,typename R, HasGT<L, R> = true>
  constexpr auto GT = [](const L &l, const R &r) ->bool {return l > r;};


  /// greater or equal
  template<typename L,typename R>
  using HasGE = std::enable_if_t <operators::has_greater_equal<L, R>::value, bool>;

  template<typename L,typename R, HasGE<L, R> = true>
  constexpr auto GE = [](const L &l, const R &r) ->bool {return l >= r;};

  ///types
  namespace relation_type {
	struct BaseType {};
	struct EQ : BaseType {};
	struct NE : BaseType {};
	struct LT : BaseType {};
	struct LE : BaseType {};
	struct GT : BaseType {};
	struct GE : BaseType {};
  }//!namespace

  using RelationTypeBase =
  		std::variant<
  		std::monostate,
  		relation_type::EQ,
  		relation_type::NE,
  		relation_type::LT,
  		relation_type::LE,
  		relation_type::GT,
  		relation_type::GE
  		>;

  struct RelationType final : public types::ObjectType<RelationTypeBase> {};

  template<typename L, typename R, typename Rl>
  constexpr bool RelationImpl (L &&l, R &&r) {
	  if constexpr	    (std::is_same_v<Rl, relation_type::EQ>) return EQ<L, R>(l, r);
	  else if constexpr (std::is_same_v<Rl, relation_type::NE>) return NE<L, R>(l, r);
	  else if constexpr (std::is_same_v<Rl, relation_type::LT>) return LT<L, R>(l, r);
	  else if constexpr (std::is_same_v<Rl, relation_type::LE>) return LE<L, R>(l, r);
	  else if constexpr (std::is_same_v<Rl, relation_type::GT>) return GT<L, R>(l, r);
	  else if constexpr (std::is_same_v<Rl, relation_type::GE>) return GE<L, R>(l, r);
	  else throw std::runtime_error (EXCEPTION_MSG("Relations type is not selected; "));
  }

  //todo: constinit instead of constexpr when moving to C++ 20 machine
//  constinit
  static const std::array<std::string, 6> relation_names {"EQ", "NE", "LT", "LE", "GT", "GE",};

  //todo: constinit instead of constexpr when moving to C++ 20 machine
  template<typename L, typename R, typename Rl>
//  constinit
  std::string RelationToString () {
  	if constexpr 	  (std::is_same_v<Rl, relation_type::EQ>) return relation_names[0]; //EQ<L, R>(l, r);
  	else if constexpr (std::is_same_v<Rl, relation_type::NE>) return relation_names[1]; //NE<L, R>(l, r);
  	else if constexpr (std::is_same_v<Rl, relation_type::LT>) return relation_names[2]; //LT<L, R>(l, r);
  	else if constexpr (std::is_same_v<Rl, relation_type::LE>) return relation_names[3]; //LE<L, R>(l, r);
  	else if constexpr (std::is_same_v<Rl, relation_type::GT>) return relation_names[4]; //GT<L, R>(l, r);
  	else if constexpr (std::is_same_v<Rl, relation_type::GE>) return relation_names[5]; //GE<L, R>(l, r);
  	else throw std::runtime_error (EXCEPTION_MSG("Relations type is not selected; "));
  }

  //todo: constinit instead of constexpr when moving to C++ 20 machine
  //  constinit
  static
  RelationType RelationFromString (const types::String str) {
  	     if (str == relation_names[0]) return RelationType{relation_type::EQ{}};
  	else if (str == relation_names[1]) return RelationType{relation_type::NE{}};
  	else if (str == relation_names[2]) return RelationType{relation_type::LT{}};
  	else if (str == relation_names[3]) return RelationType{relation_type::LE{}};
  	else if (str == relation_names[4]) return RelationType{relation_type::GT{}};
  	else if (str == relation_names[5]) return RelationType{relation_type::GE{}};
  	else throw std::runtime_error (EXCEPTION_MSG("Relations type is not selected; "));
  }


  template<typename L, typename R>
  static bool RelationImpl (L &&l, R &&r, RelationType type) {
  	     if (type.TryAs<relation_type::EQ>()) return EQ<L, R>(l, r);
  	else if (type.TryAs<relation_type::NE>()) return NE<L, R>(l, r);
  	else if (type.TryAs<relation_type::LT>()) return LT<L, R>(l, r);
  	else if (type.TryAs<relation_type::LE>()) return LE<L, R>(l, r);
  	else if (type.TryAs<relation_type::GT>()) return GT<L, R>(l, r);
  	else if (type.TryAs<relation_type::GE>()) return GE<L, R>(l, r);
  	else throw std::runtime_error (EXCEPTION_MSG("Relations type is not selected; "));
  }


}//!namespace

#endif //STRATEGY_TRADING_RELATIONS_H
