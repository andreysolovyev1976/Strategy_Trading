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


  inline char myTolower (char ch) {
  	return static_cast<char> (std::tolower(static_cast<unsigned char>(ch)));
  }

  inline std::string stringToLower (const std::string &str) {
	  std::string lower;
	  lower.reserve(str.size());
	  for (char c : str)
		  lower += myTolower(c);
	  return lower;
  }

  inline bool stringsAreEqual(const std::string &a, const std::string &b){
	  return stringToLower(a) == stringToLower(b);
  }


}



#endif //UTILS_H

