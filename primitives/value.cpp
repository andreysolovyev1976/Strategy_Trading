//
// Created by Andrey Solovyev on 03/11/2021.
//

#include "value.h"
#include <algorithm>

namespace types {
  Value::Value (BigInt &&i, BigInt &&d)
		  : integer(i)
		  , decimal(d)
		  , is_initialized (true)
		  , is_integer (false)
		  , is_rounded (false)
  {}
  Value::Value (const BigInt &i, const BigInt &d)
		  : integer(i)
		  , decimal(d)
		  , is_initialized (true)
		  , is_integer (false)
		  , is_rounded (false)
  {}

  String Value::ToString () const {
	  String output;
	  output.reserve(24);
	  if (Int() < 0) output += '-';
#ifdef _WIN32
	  throw std::invalid_argument(EXCEPTION_MSG("Should Fix conversion to WSTRING"));
  	output += boost::lexical_cast<std::wstring>(integer.str());
#else
	  output += integer.str();
#endif
	  if (IsDouble()) {
		  output += '.';
#ifdef _WIN32
		  throw std::invalid_argument(EXCEPTION_MSG("Should Fix conversion to WSTRING"));
  		output += boost::lexical_cast<std::wstring>(decimal.str());
#else
		  output += decimal.str();
#endif
	  }
	  return output;
  }

  bool operator == (const Value& lhs, const Value& rhs) {
	  return lhs.Int() == rhs.Int() &&
			  lhs.Decimal() == rhs.Decimal();
  }
  bool operator < (const Value& lhs, const Value& rhs) {
	  if (lhs.Int() == rhs.Int()) return lhs.Decimal() < rhs.Decimal();
	  else return lhs.Int() < rhs.Int();
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
	  if (
			  ((lhs.IsPositive() && rhs.IsPositive()) && (lhs.Int() > BIGINT_MAX - rhs.Int() -2)) ||
					  ((not lhs.IsPositive() && not rhs.IsPositive()) && (lhs.Int() < BIGINT_MIN - rhs.Int() + 2))
			  )
		  throw OverflowError(EXCEPTION_MSG("Overflow while adding " + lhs.ToString() + " " + rhs.ToString() + "; " ));

	  ///int case
	  BigInt i = lhs.Int() + rhs.Int();
	  if (lhs.IsInteger() && rhs.IsInteger()) return Value(i);

	  ///decimals
	  BigInt d(0);
	  if (
			  ((lhs.IsPositive() && rhs.IsPositive()) && (lhs.Decimal() > BIGINT_MAX - rhs.Decimal())) ||
					  ((not lhs.IsPositive() && not rhs.IsPositive()) && (lhs.Decimal() < BIGINT_MIN - rhs.Decimal()))
			  )
	  {
		  d.assign(lhs.Decimal()/10 + rhs.Decimal()/10);
	  }
	  else {
		  d.assign(lhs.Decimal() + rhs.Decimal());
	  }

	  return Value(std::move(i), std::move(d));
  }
  Value operator - (const Value &lhs, const Value &rhs) {
	  if (
			  ((lhs.IsPositive() && not rhs.IsPositive()) && (lhs.Int() > BIGINT_MAX - rhs.Int() - 2)) ||
					  ((not lhs.IsPositive() && rhs.IsPositive()) && (lhs.Int() < BIGINT_MIN - rhs.Int() + 2))
			  )
		  throw OverflowError(EXCEPTION_MSG("Overflow while subtracting " + lhs.ToString() + " " + rhs.ToString() + "; " ));

	  ///int case
	  BigInt i = lhs.Int() - rhs.Int();
	  if (lhs.IsInteger() && rhs.IsInteger()) return Value(i);

	  ///decimals
	  BigInt d(0);
	  if (
			  ((lhs.IsPositive() && not rhs.IsPositive()) && (lhs.Decimal() > BIGINT_MAX - rhs.Decimal())) ||
					  ((not lhs.IsPositive() && rhs.IsPositive()) && (lhs.Decimal() < BIGINT_MIN - rhs.Decimal()))
			  )
	  {
		  d.assign(lhs.Decimal()/10 - rhs.Decimal()/10);
	  }
	  else {
		  d.assign(lhs.Decimal() + rhs.Decimal());
	  }

	  return Value(std::move(i), std::move(d));
  }
  Value operator * (const Value &lhs, const Value &rhs) {
	  // (a + b) * (c + d) = a*c + a*d + b*c + b*d

	  //  	if ( abs(lhs.Int()) > BIGINT_MAX / abs(rhs.Int()) ) //Int parts overflow
	  //  		throw OverflowError(EXCEPTION_MSG("Overflow while multiplying " + lhs.ToString() + " " + rhs.ToString() + "; " ));
	  //  	BigInt a_c = lhs.Int() * rhs.Int();
	  //
	  //  	BigInt a_d = (lhs.Int() / BigInt(std::pow(10, rhs.Precision())) )  * rhs.Decimal(); //rhs.Decimal() can't have more than rhs.Precision() difits
	  //  	BigInt b_c = (rhs.Int() / BigInt(std::pow(10, lhs.Precision())) )  * lhs.Decimal(); //rhs.Decimal() can't have more than rhs.Precision() difits
	  //
	  //  	int final_precision = lhs.Precision() * rhs.Precision();
	  //
	  //  	BigInt b_d1 = lhs.Decimal() / BigInt(std::pow(10, rhs.Precision()))
	  //
	  //  	BigInt b_d =  * rhs.Decimal() / BigInt(std::pow(10, lhs.Precision()))
	  //  	///int case
	  //  	if (lhs.IsInteger() && rhs.IsInteger()) return Value(lhs.Int() * rhs.Int());
	  //
	  //  	///decimals
	  //
	  //  	BigInt result =
	  //  			lhs.Int() * rhs.Int() +
	  //  			 +
	  //  			 +
	  //  			lhs.Decimal() * BigInt(std::pow(10, rhs.Precision())) *
	  //  			;

	  return Value (lhs.Int(), rhs.Decimal());
  }

  std::ostream &operator << (std::ostream &os, const Value& r) {
	  if (not r.IsPositive()) os << '-';
	  os << r.Int();
	  if (r.IsDouble()) {
		  os << '.';
		  os << r.Decimal();
	  }
	  return os;
  }

  /*
  Value operator / (const Value &lhs, const Value &rhs) {

  }

  Value &operator += (Value &lhs, const Value &rhs) {

  }
  Value &operator -= (Value &lhs, const Value &rhs) {

  }
  Value &operator *= (Value &lhs, const Value &rhs) {

  }
  Value &operator /= (Value &lhs, const Value &rhs) {

  }

  std::istream &operator >> (std::istream &is, Value& r) {

  }
  */
}//!namespace