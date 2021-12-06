//
// Created by Andrey Solovyev on 09/11/2021.
//

#include "position.h"

namespace algo {

  types::String PositionSideToString (const PositionSide &type ) {
	  if      (type.TryAs<position_base::position_side::Neutral>()) return "Neutral";
	  else if (type.TryAs<position_base::position_side::Long>()) return "Long";
	  else if (type.TryAs<position_base::position_side::Short>()) return "Short";
	  else return "";
  }

  PositionSide StringToPositionSide (const types::String& type) {
	  if      (type == "Neutral")return PositionSide{position_base::position_side::Neutral{}};
	  else if (type == "Long") return PositionSide{position_base::position_side::Long{}};
	  else if (type == "Short") return PositionSide{position_base::position_side::Short{}};
	  else return PositionSide{};
  }

}//!namespace
