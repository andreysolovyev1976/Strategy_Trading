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
	  struct Long {};
	  struct Short {};
	}

	using PositionSideBase = std::variant<
			std::monostate,
			position_side::Long,
			position_side::Short
	>;
  }//!namespace

  struct PositionSide final : public types::ObjectType<position_base::PositionSideBase> {};

  struct Position {
	  Ticker ticker;
	  PositionSide side;
	  types::Value quantity;
  };


}//!namespace

#endif //STRATEGY_TRADING_POSITION_H
