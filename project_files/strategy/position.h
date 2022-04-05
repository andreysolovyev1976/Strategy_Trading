//
// Created by Andrey Solovyev on 09/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "object_type.h"

#include "trading_contract.h"

#ifndef STRATEGY_TRADING_POSITION_H
#define STRATEGY_TRADING_POSITION_H


namespace algo {

  namespace position_base{

	namespace position_side {
	  struct Neutral {};
	  struct Long {};
	  struct Short {};
	}//!namespace

	using PositionSideBase = std::variant<
			std::monostate,
			position_side::Neutral,
			position_side::Long,
			position_side::Short
	>;
  }//!namespace

  struct PositionSide final : public types::ObjectType<position_base::PositionSideBase> {};

  [[maybe_unused]]
  inline
  types::String PositionSideToString (const PositionSide &type ) {
	  if      (type.TryAs<position_base::position_side::Neutral>()) return "Neutral";
	  else if (type.TryAs<position_base::position_side::Long>()) return "Long";
	  else if (type.TryAs<position_base::position_side::Short>()) return "Short";
	  else return "";
  }

  [[maybe_unused]]
  inline
  PositionSide StringToPositionSide (const types::String& type) {
	  if      (type == "Neutral")return PositionSide{position_base::position_side::Neutral{}};
	  else if (type == "Long") return PositionSide{position_base::position_side::Long{}};
	  else if (type == "Short") return PositionSide{position_base::position_side::Short{}};
	  else return PositionSide{};
  }


  struct Position final {
	  Ticker ticker;
	  PositionSide side;
	  types::Value quantity;
  };


}//!namespace

#endif //STRATEGY_TRADING_POSITION_H
