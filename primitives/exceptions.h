//
// Created by Andrey Solovyev on 27/10/2021.
//

#pragma once

#include "strings.h"
#include <stdexcept>

#ifndef CPP_TEZOS_CLIENT_EXCEPTIONS_H
#define CPP_TEZOS_CLIENT_EXCEPTIONS_H

#define EXCEPTION_MSG(x) types::String(x + types::String(__PRETTY_FUNCTION__))

class ParsingError : public std::runtime_error {
public:
	using runtime_error::runtime_error;
};

class OverflowError : public std::runtime_error {
public:
	using runtime_error::runtime_error;
};


#endif //CPP_TEZOS_CLIENT_EXCEPTIONS_H
