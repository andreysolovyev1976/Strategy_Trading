//
// Created by Andrey Solovyev on 26/03/2022.
//

#include "ui_events.h"

namespace user_interface {

  Event nextEvent(Event current_event)
  {
	  if (current_event==Event::end) return current_event;
	  int curr = static_cast<int>(current_event);
	  return static_cast<Event>(++curr);
  }

}