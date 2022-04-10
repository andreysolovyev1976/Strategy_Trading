//
// Created by Andrey Solovyev on 09/04/2022.
//

#include <gtest/gtest.h>
#include "maps.h"
#include <random>
#include <future>

using namespace std;

template <typename Container> //must be [K, V]
void RunConcurrentUpdates(Container cm, size_t thread_count, int key_count) {
	auto kernel = [&cm, key_count](int seed) {
	  vector<int> updates(key_count);
	  iota(begin(updates), end(updates), -key_count / 2);
	  shuffle(begin(updates), end(updates), default_random_engine(seed));

	  for (int i = 0; i < 2; ++i) {
		  for (auto key : updates) {
			  (*cm)[key]++;
		  }
	  }
	};

	vector<future<void>> futures;
	for (size_t i = 0; i < thread_count; ++i) {
		futures.push_back(async(kernel, i));
	}
}


safe_ptr<std::map<std::string, std::pair<std::string, int> >> safe_map_strings_global;


namespace const_values {
  size_t iter_count = 1e5;
  size_t thread_count = 10;
}


void func(decltype(safe_map_strings_global) safe_map_strings)
{
	//std::lock_guard<decltype(safe_map_strings)> lock(safe_map_strings);

	using namespace const_values;

	(*safe_map_strings)["apple"].first = "fruit";
	(*safe_map_strings)["potato"].first = "vegetable";

	for (size_t i = 0; i<iter_count; ++i) {
		safe_map_strings->at("apple").second++;
		safe_map_strings->find("potato")->second.second++;
	}

	auto const readonly_safe_map_string = safe_map_strings;

#if 0
	std::cout << "potato is " << readonly_safe_map_string->at("potato").first <<
			  " " << readonly_safe_map_string->at("potato").second <<
			  ", apple is " << readonly_safe_map_string->at("apple").first <<
			  " " << readonly_safe_map_string->at("apple").second << std::endl;
#endif
}

TEST(SafePtr, BasicUsage){

	using namespace const_values;

	std::vector<std::thread> vec_thread(thread_count);
	for (auto &i : vec_thread) i = std::thread(func, safe_map_strings_global);
	for (auto &i : vec_thread) i.detach();

	std::this_thread::sleep_for(30s);

	auto const readonly_safe_map_string = safe_map_strings_global;
	ASSERT_EQ(readonly_safe_map_string->at("potato").second, iter_count * thread_count);
	ASSERT_EQ(readonly_safe_map_string->at("apple").second, iter_count * thread_count);
}

TEST(SafePtr, Update){
	using namespace const_values;

	safe_ptr<std::map<int, int>> cm;

	RunConcurrentUpdates(cm, thread_count, iter_count);
	ASSERT_EQ(cm->size(), iter_count);


	for (auto itb = cm->begin(), ite = cm->end(); itb != ite; ++itb) {
		const auto& [k, v] = *itb;
		ASSERT_EQ(v, 6) << "Key = " + to_string(k);
	}

}


template <typename C, typename K, typename V>
optional<V> Find (const C& container, K key) {
	auto found = container.find(key);
	if (found != container.end()) return optional<V> {found->second};
	else return nullopt;
}

template <typename C, typename K, typename V>
optional<V> Update (C& container, K key, V new_value) {
	auto found = container.find(key);
	if (found != container.end()){
		found->second = new_value;
		return optional<V> {found->second};
	}
	else return nullopt;
}

template <typename C>
using Map = map<string, C>;

template <typename C, typename K, typename V>
void UpdateAll ([[maybe_unused]] Map<C>& map, [[maybe_unused]] K key, [[maybe_unused]] V new_value) {

}
