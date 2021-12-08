//
// Created by Andrey Solovyev on 03/11/2021.
//

#include "value.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace types {

#define JSON_NODE_EXCEPTION_THROW \
  std::stringstream ostr; \
  Json::Document document (node); \
  Json::Print(document, ostr); \
  throw std::invalid_argument(EXCEPTION_MSG("Unable to process Json Node: " \
		  + ostr.str() \
		  + "\n"));

#define JSON_NODE_EXCEPTION_THROW_TEMP \
  throw std::invalid_argument(EXCEPTION_MSG("Unable to process Json Node: "));


  Value::Value (const String& s, int p) :number(s), precision(p) {}

  Value::Value (const Json::Node& node, int p) : precision (p) {
	  if (node.IsDict()) {
		  const auto& pair = node.AsDict();
		  if (pair.size() != 1) {
			  JSON_NODE_EXCEPTION_THROW_TEMP
		  }
		  const auto& input_json_number = pair.begin()->second;
		  if (input_json_number.IsInt()) {
			  number.assign(input_json_number.AsInt());
			  precision = 0;
		  }
		  else if (input_json_number.IsPureDouble()) {
			  number.assign(input_json_number.AsDouble());
		  }
		  else if (input_json_number.IsString()) {
			  number.assign(input_json_number.AsString());
		  }
		  else {
			  JSON_NODE_EXCEPTION_THROW_TEMP
		  }
	  }
	  else if (node.IsInt()) {
		  number.assign(node.AsInt());
		  precision = 0;
	  }
	  else if (node.IsPureDouble()) {
		  number.assign(node.AsDouble());
	  }
	  else if (node.IsString()) {
		  number.assign(node.AsString());
	  }
	  else {
		  JSON_NODE_EXCEPTION_THROW_TEMP
	  }

  }

  Value::Value (const Value &other)
		  : number (other.number)
		  , precision (other.precision)
		  , is_integer (other.is_integer)
  {}
  Value::Value (Value &&other)
		  : number (std::move(other.number))
		  , precision (other.precision)
		  , is_integer (other.is_integer)
  {}

  Value& Value::operator = (Value &&other) {
	  this->number = std::move(other.number);
	  this->precision = other.precision;
	  this->is_integer = other.is_integer;
	  return *this;
  }

  Value& Value::operator = (const Value &other) {
	  this->number = other.number;
	  this->precision = other.precision;
	  this->is_integer = other.is_integer;
	  return *this;
  }


  String Value::ToString () const {
	  String output;
	  output.reserve(24);
	  if (not IsPositive()) output += '-';
#ifdef _WIN32
	  throw std::invalid_argument(EXCEPTION_MSG("Should Fix conversion to WSTRING"));
	  output += boost::lexical_cast<std::wstring>(number_.str());
#else
	  output += number.str();
#endif
	  return output;
  }

  constexpr bool Value::IsPositive() const {
	  return number >= 0.0;
  }

  bool operator == (const Value& lhs, const Value& rhs) {
	  return lhs.number == rhs.number;
  }
  bool operator < (const Value& lhs, const Value& rhs) {
	  return lhs.number < rhs.number;
  }
  bool operator != (const Value& lhs, const Value& rhs) {
	  return not (lhs == rhs);
  }
  bool operator > (const Value& lhs, const Value& rhs) {
	  return not (lhs == rhs) && not (lhs < rhs);
  }
  bool operator <= (const Value& lhs, const Value& rhs) {
	  return not (lhs > rhs);
  }
  bool operator >= (const Value& lhs, const Value& rhs) {
	  return not (lhs < rhs);
  }

  Value operator + (const Value &lhs, const Value &rhs) {
	  Float new_number_ = lhs.number + rhs.number;
	  auto precision_ = std::max(lhs.precision, rhs.precision);
	  return Value(new_number_, precision_);
  }
  Value operator - (const Value &lhs, const Value &rhs) {
	  Float new_number_ = lhs.number - rhs.number;
	  auto precision_ = std::max(lhs.precision, rhs.precision);
	  return Value(new_number_, precision_);
  }
  Value operator * (const Value &lhs, const Value &rhs) {
	  Float new_number_ = lhs.number * rhs.number;
	  auto precision_ = std::max(lhs.precision, rhs.precision);
	  return Value(new_number_, precision_);
  }

  Value operator / (const Value &lhs, const Value &rhs) {
	  Float new_number_ = lhs.number / rhs.number;
	  auto precision_ = std::max(lhs.precision, rhs.precision);
	  return Value(new_number_, precision_);
  }

  Value &operator += (Value &lhs, const Value &rhs) {
	  lhs.number += rhs.number;
	  return lhs;
  }
  Value &operator -= (Value &lhs, const Value &rhs) {
	  lhs.number -= rhs.number;
	  return lhs;
  }
  Value &operator *= (Value &lhs, const Value &rhs) {
	  lhs.number *= rhs.number;
	  return lhs;
  }
  Value &operator /= (Value &lhs, const Value &rhs) {
	  lhs.number /= rhs.number;
	  return lhs;
  }

  std::ostream &operator << (std::ostream &os, const Value& r) {
	  os << std::setprecision(r.precision) << std::fixed << r.number;
	  return os;
  }

  std::istream &operator >> (std::istream &is, Value& r) {
	  r = Value(0); //todo: check this operator
	  return is;
  }

}//!namespace