//
// Created by Andrey Solovyev on 02/11/2021.
//

#pragma once

#include "types_declarations.h"
#include "relations.h"
#include "quote.h"
#include <boost/circular_buffer.hpp>

#include <type_traits>
#include <memory>
#include <ostream>

#ifndef STRATEGY_TRADING_INDICATOR_H
#define STRATEGY_TRADING_INDICATOR_H

class value;
namespace algo {

  namespace indicator_base {

//	template <typename IndicatorType>
//	struct IsIndicatorType {
//		constexpr static bool value =
//				std::disjunction_v<
//				std::is_same<types::Value, IndicatorType>,
//				std::is_same<std::vector<types::Value>, IndicatorType>,
//				std::is_same<algo::Quote, IndicatorType>,
//				std::is_same<std::vector<algo::Quote>, IndicatorType>
//		>;
//	};
//
//	template<typename T>
//	constexpr bool IsIndicatorType_v(T) {
//		return IsIndicatorType<std::decay_t<T>>::value;
//	}

	template <typename IndicatorType>
	using IsIndicator = std::enable_if_t<
			std::disjunction_v<
					std::is_same<types::Value, IndicatorType>,
					std::is_same<std::vector<types::Value>, IndicatorType>,
					std::is_same<algo::Quote, IndicatorType>,
					std::is_same<std::vector<algo::Quote>, IndicatorType>
			>,
			bool>;

	template<typename Modifier, typename IndicatorType, IsIndicator<IndicatorType> = true>
	using IsCallable = std::enable_if_t<
			std::disjunction_v<
					std::is_invocable_r<IndicatorType, Modifier, IndicatorType>,
					std::is_same<Modifier, nullptr_t>,
					std::is_same<Modifier, std::unique_ptr<std::is_invocable_r<IndicatorType, Modifier, IndicatorType>>>
			>, bool>;

  }//!namespace


  template <typename IndicatorType, typename ModifierFunc = nullptr_t>
  class Indicator final {
  public:
	  Indicator() = default;
	  Indicator (const Indicator &other) = delete;

	  template <indicator_base::IsCallable<ModifierFunc, IndicatorType> = true>
	  Indicator(const types::String& label, IndicatorType input_value, ModifierFunc modifier = nullptr)
			  :label_(label)
			  , input_value_(std::move(input_value))
			  , modifier(modifier)
	  {
		  ProcessIndicator();
		  data.resize(30);
	  }

	  template<typename NewIndicatorType, indicator_base::IsIndicator<NewIndicatorType> = true>
	  	void updateInputValue (NewIndicatorType new_value) {
	  		if constexpr (sameType(input_value_, new_value)){
			  input_value_ = std::move(new_value);
			  ProcessIndicator();
		  }
	  		else if constexpr (typeAndItsVector(new_value, input_value_)) {
	  			input_value_.emplace_back(std::move(new_value));
			  ProcessIndicator();
		  }
	  }
	  const IndicatorType& getOutputValue() const {return output_value_;}
	  const IndicatorType& getInputValue() const {return input_value_;}

	  const types::String& getLabel () const {return label_;}

  private:
	  types::String label_;
	  IndicatorType input_value_, output_value_;
	  ModifierFunc modifier;
	  boost::circular_buffer<IndicatorType> data;

	  void ProcessIndicator () {
		  if constexpr (not std::is_same_v<std::decay_t<decltype(modifier)>, nullptr_t>){
			  output_value_ = modifier(input_value_);
		  }
		  else {
			  output_value_ = input_value_;
		  }
	  }

	  template <typename A, typename B>
	  constexpr bool sameType (A, B){
	  	return std::is_same_v<std::decay_t<A>, std::decay_t<B>>;
	  }
	  template <typename A, typename Vec>
	  constexpr bool typeAndItsVector (A, Vec){
	  	return std::is_same_v<std::decay_t<A>, typename std::decay_t<Vec>::value_type>;
	  }
  };

  template <typename IndicatorType, typename Modifier>
  std::ostream& operator<<(std::ostream& os, const Indicator<IndicatorType, Modifier> &indicator) {
	  using IndicatorValueType = std::decay_t<decltype(indicator.getOutputValue())>;
	  if constexpr (std::is_same_v<IndicatorValueType, types::Value>){
		  os << "label: " << indicator.getLabel() << " indicator: " << indicator.getOutputValue();
	  }
	  else if constexpr (std::is_same_v<IndicatorValueType, algo::Quote>){
		  os << "label: " << indicator.getLabel() << " indicator: {" << indicator.getOutputValue() << "}";
	  }
	  else {
		  os << "Indicator is vector, operator << needs to be updated;\n";
	  }
	  return os;
  }

}//!namespace


#endif //STRATEGY_TRADING_INDICATOR_H
