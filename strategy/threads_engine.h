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

  template<typename Label, IsMappable<Label> = true>
  class Engine {
	  using Thread = boost::thread;
	  using ThreadID = boost::thread::id;
	  using ThreadMap = std::map<ThreadID, Thread>;
	  using ThreadMapIter = typename std::map<ThreadID, Thread>::iterator;
	  using LabelIdMap = std::map<Label, ThreadID>;
	  using IdLabelMap = std::map<ThreadID, Label>;

  public:
	  explicit Engine() = default;
	  Engine(const Engine&) = delete;
	  Engine operator=(const Engine&) = delete;
	  void addThread(Thread&& thread, Label label)
	  {
		  auto id = thread.get_id();
		  thread_map.insert(std::make_pair(id, std::move(thread)));
		  label_to_id.insert(std::make_pair(label, id));
		  id_to_label.insert(std::make_pair(id, std::move(label)));
		  thread_map.at(id).detach();
	  }
	  template<typename F, typename... Args>
	  void createThread(Label label, F func, Args... args)
	  {
		  boost::thread _(func, args...);
		  this->addThread(std::move(_), std::move(label));
	  }

	  bool interruptThread(ThreadID id)
	  {
		  if (auto found_thread = thread_map.find(id); found_thread!=thread_map.end()) {
			  __eraseThread(found_thread);
			  return true;
		  }
		  else {
			  return false;
		  }
	  }

	  bool interruptThread(Label label)
	  {
		  if (auto found_label = label_to_id.find(label); found_label!=label_to_id.end()) {
			  const auto& id = found_label.second;
			  if (auto found_thread = thread_map.find(id); found_thread!=thread_map.end()) {
				  __eraseThread(found_thread);;
				  return true;
			  }
		  }
		  else {
			  return false;
		  }
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
		  Thread tr(&Engine::__eraseThreadHelper, this, std::move(found->second));
		  tr.detach();
		  auto id_label = id_to_label.find(found->first);
		  label_to_id.erase(id_label->second);
		  id_to_label.erase(id_label);
		  thread_map.erase(found);
	  }
	  void __eraseThreadHelper(Thread&& thread)
	  {
		  thread.interrupt();
	  }
  };
}//!namespace
#endif //STRATEGY_TRADING_THREADS_ENGINE_H
