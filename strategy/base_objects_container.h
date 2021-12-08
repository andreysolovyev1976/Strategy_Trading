//
// Created by Andrey Solovyev on 08/12/2021.
//

#pragma once

#include "maps.h"
#include "types_declarations.h"
#include "exceptions.h"

#include <memory>
#include <string_view>

#ifndef STRATEGY_TRADING_BASE_OBJECTS_CONTAINER_H
#define STRATEGY_TRADING_BASE_OBJECTS_CONTAINER_H

template <typename T>
class Objects {
	using Owner = std::unique_ptr<T>;
	using Ptr = T*;
	//todo: add thread safe map
	using ByLabel = types::SingleThreadedLimitedSizeMap<std::string_view, Owner>;
	using ByTicker = types::SingleThreadedMultiMap<std::string_view, Ptr>;
public:
	Objects();
	const ByLabel& getByLabel () const;
	const ByTicker& getByTicker () const;
	ByLabel& getByLabel ();
	ByTicker& getByTicker ();
	const T& getObject (const types::String &label) const;
	Ptr shareObject (const types::String &label);

	virtual ~Objects();

private:
	///owner of the data
	ByLabel by_label_;
	///shallow copies
	ByTicker by_ticker_;
};

template <typename T>
Objects<T>::Objects()
		: by_label_ ( types::makeSingleThreadedLimitedSizeMap<std::string_view, Owner>())
		, by_ticker_(types::makeSingleThreadedMultiMap<std::string_view, Ptr>())
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
		throw std::invalid_argument(EXCEPTION_MSG("Unknown signal label; "));
	}
	else return *found->second;
}

template <typename T>
typename Objects<T>::Ptr Objects<T>::shareObject (const types::String &label) {
	if (auto found = by_label_->Find(label); found==by_label_->end())
		throw std::invalid_argument(EXCEPTION_MSG("Unknown signal label; "));
	else return &*found->second;
}

#endif //STRATEGY_TRADING_BASE_OBJECTS_CONTAINER_H
