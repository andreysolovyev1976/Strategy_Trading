//
// Created by Andrey Solovyev on 08/12/2021.
//

#pragma once

#include "maps.h"
#include "types_declarations.h"
#include "user_defined_exceptions.h"

#ifndef STRATEGY_TRADING_BASE_OBJECTS_CONTAINER_H
#define STRATEGY_TRADING_BASE_OBJECTS_CONTAINER_H

template<typename T>
using Ptr = safe_ptr<T>;
//using Ptr = std::shared_ptr<T>;

template <typename T>
class Objects {
public:
	using ByLabel = types::MultiThreadedMap<types::String, Ptr<T>>;

	Objects();
	Objects(const Objects<T>& other_objects) = delete;
	Objects<T> operator = (const Objects<T>& other_objects) = delete;
	virtual ~Objects() = default;

	const ByLabel& getByLabel () const;
	ByLabel& getByLabel ();

/*
	const T& getObject (const types::String &label) const;
	T& getObject (const types::String &label);
*/
	bool objectExists (const types::String &label) const;
	Ptr<T> getSafePtr (const types::String &label);

	//todo: should I go with this design?
//	virtual void addObject (T object) = 0;
//	virtual void removeObject (const types::String &label) = 0;

private:
	///owners of the data
	ByLabel by_label_;
};

template<typename T>
Objects<T>::Objects() : by_label_ (types::MultiThreadedMap<types::String, Ptr<T>>())
{}


template <typename T>
const typename Objects<T>::ByLabel& Objects<T>::getByLabel () const {return by_label_;}

template <typename T>
typename Objects<T>::ByLabel& Objects<T>::getByLabel () {return by_label_;}

/*
template <typename T>
const T& Objects<T>::getObject (const types::String &label) const {
	if (auto found = by_label_->find(label); found == by_label_->end()){
		throw std::invalid_argument(EXCEPTION_MSG("Unknown Object label: " + label + "; "));
	}
	else return (found->second);
}

template <typename T>
T& Objects<T>::getObject (const types::String &label) {
	if (auto found = by_label_->find(label); found == by_label_->end()){
		throw std::invalid_argument(EXCEPTION_MSG("Unknown Object label: " + label + "; "));
	}
	else return (found->second);
}
*/

template <typename T>
bool Objects<T>::objectExists (const types::String &label) const {
	auto found = by_label_->find(label);
	return found != by_label_->end();
}


template <typename T>
Ptr<T> Objects<T>::getSafePtr (const types::String &label) {
	if (auto found = by_label_->find(label); found == by_label_->end()){
		throw std::invalid_argument(EXCEPTION_MSG("Unknown Object label: " + label + "; "));
	}
	else return found->second;
//	else return std::shared_ptr<T>(found->second.get(), [](auto *){/* do nothing */}) ;
}

#endif //STRATEGY_TRADING_BASE_OBJECTS_CONTAINER_H
