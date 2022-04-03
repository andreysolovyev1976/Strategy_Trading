//
// Created by Andrey Solovyev on 08/12/2021.
//

#pragma once

#include "maps.h"
#include "types_declarations.h"
#include "user_defined_exceptions.h"

#include <memory>

#ifndef STRATEGY_TRADING_BASE_OBJECTS_CONTAINER_H
#define STRATEGY_TRADING_BASE_OBJECTS_CONTAINER_H

//todo UPDATE for multithreading!!!!

template <typename T>
struct Labels {
	using ByLabel = types::SingleThreadedLimitedSizeMap<types::String, std::unique_ptr<T>>;
	Labels();
	virtual ~Labels() = default;

	///owners of the data
	ByLabel by_label_;
};

template <typename T>
Labels<T>::Labels()
		: by_label_ ( types::makeSingleThreadedLimitedSizeMap<types::String, std::unique_ptr<T>>())
{}


template <typename T>
struct Tickers {
	using ByTicker = types::SingleThreadedMultiMap<types::String, T*>;
	Tickers();
	virtual ~Tickers() = default;

	///shallow copies
	ByTicker by_ticker_;
};

template <typename T>
Tickers<T>::Tickers()
		: by_ticker_ ( types::makeSingleThreadedLimitedSizeMap<types::String, T*>())
{}


template <typename T>
class Objects_ {
public:
	Objects_();
	virtual ~Objects_() = default;

	const T& getObject (const types::String &label) const;
	T* shareObject (const types::String &label);

	virtual void addObject (T object) = 0;
	virtual void removeObject (const types::String &label) = 0;

private:
	Labels<T> labels_;
	Tickers<T> tickers_;
};


template <typename T>
const T& Objects_<T>::getObject (const types::String &label) const {
	if (auto found = this->labels_.by_label_ ->Find(label); found == labels_.by_label_->end()){
		throw std::invalid_argument(EXCEPTION_MSG("Unknown Object label: " + label + "; "));
	}
	else return *found->second;
}

template <typename T>
T* Objects_<T>::shareObject (const types::String &label) {
	if (auto found = labels_.by_label_->Find(label); found==labels_.by_label_->end())
		throw std::invalid_argument(EXCEPTION_MSG("Unknown Object label: " + label + "; "));
	else return &*found->second;
}


template <typename T>
class Objects {
public:
	using Owner = std::unique_ptr<T>;
	using Ptr = T*;
	//todo: add thread safe map, and it shouldn't be LIMITED size
	using ByLabel = types::SingleThreadedLimitedSizeMap<types::String, Owner>;
	using ByTicker = types::SingleThreadedMultiMap<types::String, Ptr>;

	Objects();
	Objects(Objects<T>&& other_objects);
	Objects(const Objects<T>& other_objects) = delete;

	const ByLabel& getByLabel () const;
	const ByTicker& getByTicker () const;
	ByLabel& getByLabel ();
	ByTicker& getByTicker ();
	const T& getObject (const types::String &label) const;
	T* getPtr (const types::String &label);
	Ptr shareObject (const types::String &label);

	//todo: should I go with this design?
//	virtual void addObject (T object) = 0;
//	virtual void removeObject (const types::String &label) = 0;

	virtual ~Objects() = default;

private:
	///owner of the data
	ByLabel by_label_;
	///shallow copies
	ByTicker by_ticker_;
};

template <typename T>
Objects<T>::Objects()
		: by_label_ ( types::makeSingleThreadedLimitedSizeMap<types::String, Owner>())
		, by_ticker_(types::makeSingleThreadedMultiMap<types::String, Ptr>())
{}

template <typename T>
Objects<T>::Objects(Objects<T>&& other_objects)
		: by_label_ (std::move(other_objects.by_label_))
		, by_ticker_(std::move(other_objects.by_ticker_))
{}


template <typename T>
const typename Objects<T>::ByLabel& Objects<T>::getByLabel () const {return by_label_;}
template <typename T>
const typename Objects<T>::ByTicker& Objects<T>::getByTicker () const {return by_ticker_;}
template <typename T>
typename Objects<T>::ByLabel& Objects<T>::getByLabel () {return by_label_;}
template <typename T>
typename Objects<T>::ByTicker& Objects<T>::getByTicker () {return by_ticker_;}

template <typename T>
const T& Objects<T>::getObject (const types::String &label) const {
	if (auto found = by_label_->Find(label); found == by_label_->end()){
		throw std::invalid_argument(EXCEPTION_MSG("Unknown Object label: " + label + "; "));
	}
	else return *found->second;
}

template <typename T>
T* Objects<T>::getPtr (const types::String &label) {
	if (auto found = by_label_->Find(label); found == by_label_->end()){
		return nullptr;
	}
	else return found->second.get();
}

template <typename T>
typename Objects<T>::Ptr Objects<T>::shareObject (const types::String &label) {
	if (auto found = by_label_->Find(label); found==by_label_->end())
		throw std::invalid_argument(EXCEPTION_MSG("Unknown Object label: " + label + "; "));
	else return &*found->second;
}

#endif //STRATEGY_TRADING_BASE_OBJECTS_CONTAINER_H
