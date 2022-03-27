//
// Created by Andrey Solovyev on 09/11/2021.
//

#pragma once



#include <string>
#include <algorithm>
#include <cctype>
#include <type_traits>
#include <vector>
#include <charconv>
#include <stdexcept>

#ifndef STRATEGY_TRADING_STRING_H
#define STRATEGY_TRADING_STRING_H

namespace types {

#ifdef _WIN32
  using String = std::wstring;
#else
  using String = std::string;
#endif

  inline String trim(String &s) {
	  auto from = std::find_if(s.begin(), s.end(), [](int c) { return !std::isspace(c); });
	  auto to = std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base();

	  return (to > from) ? String(from, to) : "";
  }

  template <typename... T>
  String concatenateStrings(char delim, const T... sources){
	  String result;
	  ((result += sources + delim), ...);
	  result.pop_back();
	  return result;
  }//!func


  inline
  std::vector<String> splitIntoWords(const String &line, char delim) {
	  if (line.empty())
		  return {};
	  std::vector<std::string> result;
	  result.reserve(2);
	  size_t
			  current_pos = 0,
			  line_size = line.size();
	  while (true) {
		  size_t delim_pos = line.find(delim, current_pos);
		  if (delim_pos == line.npos)
			  delim_pos = line_size;
		  int substr_length = delim_pos - current_pos;
		  if (substr_length < 0)
			  throw std::runtime_error("substr length is less than zero: " + std::string(__PRETTY_FUNCTION__));
		  auto sub = line.substr(current_pos, substr_length);
		  if (!sub.empty())
			  result.push_back(std::move(sub));
		  if (delim_pos == line_size)
			  break;
		  else
			  current_pos = ++delim_pos;
	  } //!while
	  return result;
  } //!func


  [[maybe_unused]]
  static
  int64_t fromChars(const std::string &str_int) {
	  int64_t local_result{};
	  auto [ptr, ec]{std::from_chars(str_int.data(), str_int.data() + str_int.size(), local_result)};
	  if (ec == std::errc::invalid_argument) {
		  throw std::invalid_argument("Attempt to convert not a number");
	  } else if (ec == std::errc::result_out_of_range) {
		  throw std::invalid_argument("Out of bound for an int64_t; ");
	  }
	  return local_result;
  }


}//!namespace


#endif //STRATEGY_TRADING_STRING_H
