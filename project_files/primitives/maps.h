//
// Created by Andrey Solovyev on 09/11/2021.
//

#pragma once

#include <map>
#include <string>
#include "safe_ptr.h"
#include "user_defined_exceptions.h"

#ifndef STRATEGY_TRADING_MAPS_H
#define STRATEGY_TRADING_MAPS_H

namespace types {

  //todo: move this const - 30 to const_declarations
  //todo: add custom comparator
  template <typename K, typename V, size_t N = 60>
  class MapLimitedSize {
  private:
	  using Pair = std::pair<K, V>;
	  using Iterator = typename std::map<K, V>::iterator;
	  using ConstIterator = typename std::map<K, V>::const_iterator;
	  using ReverseIterator = typename std::map<K, V>::reverse_iterator;
	  using ConstReverseIterator = typename std::map<K, V>::const_reverse_iterator;
	  struct InsertResult {
		  Iterator iter;
		  bool result;
	  };
  public:
	  MapLimitedSize() = default;
	  MapLimitedSize(std::map<K, V> new_container) : data(std::move(new_container)) {}

	  size_t size() const {
	  	return data.size();
	  }
	  bool empty() const {
		  return data.empty();
	  }
	  ConstIterator find(K key) const {
		  return data.find(key);
	  }
	  Iterator find(K key) {
		  return data.find(key);
	  }

	  V& operator [] (K key) = delete;
	  V& operator [] (const K &key) = delete;

	  V& at(K key) {
	  	if (auto found = data.find(key); found == data.end()) {
			throw std::invalid_argument(EXCEPTION_MSG("No such Key in Map; "));
		}
		  else return found->second;
	  }

	  InsertResult insert (K key, V value) {
		  while (data.size() >= max_size) {
			  data.erase(data->begin());
		  }
		  auto res = data.insert(make_pair(key, value));
		  return InsertResult {.iter = res.first, .result = res.second};
	  }

	  InsertResult insert (Pair pair) {
		  while (data.size() >= max_size) {
			  data.erase(data.begin());
		  }
		  auto res = data.insert(std::move(pair));
		  return InsertResult {.iter = res.first, .result = res.second};
	  }

	  void erase (K key) {
	  	data.erase(key);
	  }

	  void erase (Iterator it) {
		  data.erase(it);
	  }
	  void erase (ReverseIterator it) {
		  data.erase(it);
	  }

	  Iterator begin () {
		  return data.begin();
	  }
	  ConstIterator begin () const { //todo: stupid as I need this for range-based for. Change this.
		  return data.begin();
	  }
	  ConstIterator cbegin () const {
		  return data.begin();
	  }
	  ReverseIterator rbegin () {
		  return data.rbegin();
	  }
	  ConstReverseIterator crbegin () const {
		  return data.crbegin();
	  }
	  Iterator end () { //todo: stupid as I need this for range-based for. Change this.
		  return data.end();
	  }
	  ConstIterator end () const { //todo: stupid as I need this for range-based for. Change this.
		  return data.end();
	  }
	  ConstIterator cend () const {
		  return data.end();
	  }
	  ReverseIterator rend () {
		  return data.rend();
	  }
	  ConstReverseIterator crend () const {
		  return data.crend();
	  }
  private:
	  std::map<K, V> data;
	  const size_t max_size = N;
  };

  template <typename K, typename V>
  using SingleThreadedLimitedSizeMap = std::unique_ptr<MapLimitedSize<K, V>>;
  template <typename K, typename V>
  SingleThreadedLimitedSizeMap<K, V> makeSingleThreadedLimitedSizeMap (){
  	return std::make_unique<MapLimitedSize<K, V>>();
  }

  template <typename K, typename V>
  using MultiThreadedLimitedSizeMap = safe_ptr<MapLimitedSize<K, V>>;
  template <typename K, typename V>
  MultiThreadedLimitedSizeMap<K, V> makeMultiThreadedLimitedSizeMap () {
	safe_ptr<MapLimitedSize<K, V>> _;
	return _;
  }

  template <typename K, typename V>
  using SingleThreadedMultiMap = std::unique_ptr<std::multimap<K, V>>;
  template <typename K, typename V>
  SingleThreadedMultiMap<K, V> makeSingleThreadedMultiMap (){
	return std::make_unique<std::multimap<K, V>>();
  }

  template <typename K, typename V>
  using MultiThreadedMultiMap = safe_ptr<std::multimap<K, V>>;
  template <typename K, typename V>
  MultiThreadedMultiMap<K,V> makeMultiThreadedMultiMap (){
	safe_ptr<std::multimap<K, V>> _;
	return _;
  }

  template <typename K, typename V>
  using SingleThreadedMap = std::unique_ptr<std::map<K, V>>;
  template <typename K, typename V>
  using MultiThreadedMap = safe_ptr<std::map<K, V>>;



}//!namespace

#endif //STRATEGY_TRADING_MAPS_H
