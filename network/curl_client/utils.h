//
// Created by Andrey Solovyev on 27/09/2021.
//

#pragma once

#include <vector>
#include <string>

#ifndef UTILS_H
#define UTILS_H

namespace utils {

  std::vector <std::string> splitIntoWords(const std::string &line, char delim);

  // helper type for the visitor #4
  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
  // explicit deduction guide (not needed as of C++20)
  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}



#endif //UTILS_H

