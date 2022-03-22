//
// Created by Andrey Solovyev on 22/03/2022.
//

#include "os_pipe.h"
#include "user_defined_exceptions.h"

#include <cstdio>
#include <array>
#include <memory>

namespace os {

  void invokeCommandAndCaptureResult(const char* cmd, types::String& result) {
	  std::array<char, 512> buffer;
	  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

	  if (!pipe) {
		  throw RuntimeError("popen() failed!");
	  }

	  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		  result += buffer.data();
	  }
  }//!func


}//!namespace
