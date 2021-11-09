//
// Created by Andrey Solovyev on 09/11/2021.
//

#pragma once

#include <map>

#ifndef STRATEGY_TRADING_MAPS_H
#define STRATEGY_TRADING_MAPS_H

namespace types {

  template <typename K, typename V>
  using SingleThreadedMap = std::map<K, V>;

  template <typename K, typename V>
  using MultiThreadedMap = std::map<K, V>;


  template <typename K, typename V>
  using MultiMap = std::multimap<K, V>;


}//!namespace

#endif //STRATEGY_TRADING_MAPS_H
