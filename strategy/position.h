//
// Created by Andrey Solovyev on 09/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "object_type.h"

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
  static types::String PositionSideToString (const PositionSide &type);

  [[maybe_unused]]
  PositionSide StringToPositionSide (const types::String& type);

  struct Position final {
	  Ticker ticker;
	  PositionSide side;
	  types::Value quantity;
  };


}//!namespace

#endif //STRATEGY_TRADING_POSITION_H
