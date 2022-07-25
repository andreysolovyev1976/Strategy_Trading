//
// Created by Andrey Solovyev on 30/03/2022.
//

#pragma once

#include "operators.h"

#include <boost/thread/thread.hpp>
#include <map>

#ifndef STRATEGY_TRADING_THREADS_ENGINE_H
#define STRATEGY_TRADING_THREADS_ENGINE_H

namespace threads {
/// less than
  template<typename T>
  using IsMappable = std::enable_if_t<operators::has_less_than<T, T>::value, bool>;
  using Thread = boost::thread; //todo: exchange for std::jthread from C++20

  template<typename Label, IsMappable<Label> = true>
  class Engine {
  private:
	  using ThreadID = boost::thread::id;
	  using ThreadMap = std::map<ThreadID, Thread>;
	  using ThreadMapIter = typename std::map<ThreadID, Thread>::iterator;
	  using LabelIdMap = std::map<Label, ThreadID>;
	  using IdLabelMap = std::map<ThreadID, Label>;

  public:
	  explicit Engine() = default;
	  Engine(const Engine&) = delete;
	  Engine operator=(const Engine&) = delete;
//	  ~Engine() {
//	  	for (auto& [id, t] : thread_map) {
//			t.join();
//		}
//	  }
	  void addThread(Label label, Thread&& thread){
		  auto id = thread.get_id();
		  thread_map.insert(std::make_pair(id, std::move(thread)));
		  label_to_id.insert(std::make_pair(label, id));
		  id_to_label.insert(std::make_pair(id, std::move(label)));
		  thread_map.at(id).join();
	  }
	  template<typename F, typename... Args>
	  void createThread(Label label, F func, Args... args){
		  boost::thread _(func, std::forward<Args>(args)...);
		  this->addThread(std::move(label), std::move(_));
	  }

	  //todo: need to introduce interruption point as a conditional_variable
	  bool interruptThread(Label label){
		  if (auto found_label = label_to_id.find(label); found_label!=label_to_id.end()) {
			  const auto& id = found_label->second;
			  if (auto found_thread = thread_map.find(id); found_thread!=thread_map.end()) {
				  __eraseThread(found_thread);;
				  return true;
			  }
		  }
		  return false;
	  }

	  size_t size() const { return __size(); }

  private:
	  ThreadMap thread_map;
	  LabelIdMap label_to_id;
	  IdLabelMap id_to_label;
	  size_t __size()
	  {
		  if (id_to_label.size()!=label_to_id.size()) {
			  throw std::runtime_error("Map sizes are not equal, can't proceed.");
		  }
		  else {
			  return id_to_label.size();
		  }
	  }

	  void __eraseThread(ThreadMapIter found)
	  {
		  found->second.interrupt();
		  auto id_label = id_to_label.find(found->first);
		  label_to_id.erase(id_label->second);
		  id_to_label.erase(id_label);
		  thread_map.erase(found);
	  }
  };
}//!namespace
#endif //STRATEGY_TRADING_THREADS_ENGINE_H
