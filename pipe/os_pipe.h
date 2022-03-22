//
// Created by Andrey Solovyev on 22/03/2022.
//

#pragma once

#include "user_defined_strings.h"

#ifndef STRATEGY_TRADING_OS_PIPE_H
#define STRATEGY_TRADING_OS_PIPE_H

namespace os {

  void invokeCommandAndCaptureResult(const char* cmd, types::String& result);

}//!namespace

#endif //STRATEGY_TRADING_OS_PIPE_H
