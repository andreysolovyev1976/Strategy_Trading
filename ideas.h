//
// Created by Andrey Solovyev on 08/11/2021.
//

#pragma once
#ifndef STRATEGY_TRADING_IDEA_H
#define STRATEGY_TRADING_IDEA_H

///idicator
namespace algo {

  namespace indicator_base {

    template <typename Data>
    using BaseContainer = boost::circular_buffer<Data>;

    template <typename IndicatorType>
    using IsIndicator = std::enable_if_t<
    		std::disjunction_v<
    		std::is_same<types::Value, IndicatorType>,
    		std::is_same<algo::Quote, IndicatorType>
    		>,
    		bool>;

    template<typename Modifier, typename IndicatorType, IsIndicator<IndicatorType> = true>
    		using IsCallable = std::enable_if_t<
    				std::disjunction_v<
    				std::is_invocable_r<BaseContainer<IndicatorType>, Modifier, BaseContainer<IndicatorType>>,
    				std::is_same<Modifier, nullptr_t>,
    				std::is_same<Modifier, std::unique_ptr<std::is_invocable_r<BaseContainer<IndicatorType>, Modifier, BaseContainer<IndicatorType>>>>
    				>, bool>;

    class IndicatorBase {
    public:
    	virtual ~IndicatorBase() {};
    };

  }//!namespace


  template <typename IndicatorType, typename ModifierFunc = nullptr_t>
  		class Indicator final : public indicator_base::IndicatorBase {
  public:
  	Indicator() = default;
  	//	  Indicator (const Indicator &other) = delete;

  	template <indicator_base::IsCallable<ModifierFunc, IndicatorType> = true>
  			Indicator(types::String indicator_label, indicator_base::BaseContainer<IndicatorType> input_value, ModifierFunc modifier = nullptr)
  			: label_(indicator_label)
  			, modifier(modifier)
  			, input_value_(std::move(input_value))
  			{
  				this->ProcessIndicator();
  			}

  			template <indicator_base::IsCallable<ModifierFunc, IndicatorType> = true>
  					Indicator(types::String indicator_label, ModifierFunc modifier = nullptr)
  					: label_(indicator_label)
  					, modifier(modifier)
  					{
  						input_value_.resize(default_size_);
  						this->ProcessIndicator();
  					}

  					template<typename NewIndicatorType, indicator_base::IsIndicator<NewIndicatorType> = true>
  							void updateInputValues (NewIndicatorType new_value) {
  								if constexpr (sameType(input_value_, new_value)){
  									input_value_ = std::move(new_value);
  								}
  								else if constexpr (typeAndItsContainer(new_value, input_value_)) {
  									input_value_.push_back(std::move(new_value));
  								}
  								else {
  									throw std::invalid_argument (EXCEPTION_MSG("Arg type mismatch; "));
  								}
  								this->ProcessIndicator();
  							}
  							const indicator_base::BaseContainer<IndicatorType>& getOutputValues() const {return output_value_;}
  							const indicator_base::BaseContainer<IndicatorType>& getInputValues() const {return input_value_;}

  							const types::String& getLabel () const {return label_;}

  							bool Empty () const {
  								if constexpr (not std::is_same_v<ModifierFunc, nullptr_t>){
  									constexpr bool ret = input_value_.empty();
  									return ret;
  								}
  								else {
  									constexpr bool ret = input_value_.empty() && output_value_.empty();
  									return ret;
  								}
  							}

  private:
  	types::String label_;
  	ModifierFunc modifier;
  	indicator_base::BaseContainer<IndicatorType> input_value_, output_value_;
  	[[maybe_unused]] size_t default_size_ = const_values::DEFAULT_INDICATOR_SIZE;

  	void ProcessIndicator () {
  		if constexpr (not std::is_same_v<ModifierFunc, nullptr_t>){
  			output_value_ = modifier(input_value_);
  		}
  		else {
  			output_value_ = std::move(input_value_);
  		}
  	}

  	template <typename A, typename B>
  	constexpr bool sameType (A, B){
  		return std::is_same_v<std::decay_t<A>, std::decay_t<B>>;
  	}
  	template <typename A, typename Vec>
  	constexpr bool typeAndItsContainer (A, Vec){
  		return std::is_same_v<std::decay_t<A>, typename std::decay_t<Vec>::value_type>;
  	}
  };

  template <typename IndicatorType, typename Modifier>
  std::ostream& operator<<(std::ostream& os, const Indicator<IndicatorType, Modifier> &indicator) {
  	os << "label: " << indicator.getLabel() << '\n';
  	const auto &indicator_values = indicator.getOutputValues();
  	for (const auto &value : indicator_values) {
  		os << value << '\n';
  	}
  	return os;
  }

}//!namespace



namespace rule_base {

  namespace rule_type {
    struct Buy {};
    struct Sell {};
    struct DoNothing {};
  }//!namespace

  template <typename R>
  using IsRule = std::enable_if_t<
  		std::disjunction_v<
  		std::is_same<R, rule_type::Buy>,
  		std::is_same<R, rule_type::Sell>,
  		std::is_same<R, rule_type::DoNothing>
  		>, bool>;

  template <typename R1, typename R2, IsRule<R1> = true, IsRule<R2> = true>
  		bool constexpr IsSame (R1 lhs, R2 rhs) {
  			return std::is_same_v<std::decay_t<decltype(lhs)>, std::decay_t<decltype(rhs)>>;
  		}

  		template <typename R1, typename R2, IsRule<R1> = true, IsRule<R2> = true>
  				bool constexpr IsNotSame (R1 lhs, R2 rhs) {
  					return not IsSame(lhs, rhs);
  				}

  				using RuleTypeBase =
  						std::variant<
  						std::monostate,
  						rule_type::Buy,
  						rule_type::Sell,
  						rule_type::DoNothing
  						>;

  struct RuleType final : public types::ObjectType<rule_base::RuleTypeBase> {};

}//!namespace



namespace rule_base {

  namespace rule_type {
    struct Buy {};
    struct Sell {};
    struct DoNothing {};
  }//!namespace

  using RuleTypeBase =
  		std::variant<
  		std::monostate,
  		rule_type::Buy,
  		rule_type::Sell,
  		rule_type::DoNothing
  		>;

  struct RuleType final : public types::ObjectType<rule_base::RuleTypeBase> {};

}//!namespace


#endif //STRATEGY_TRADING_IDEA_H
