//
// Created by Andrey Solovyev on 30/05/2022.
//

#include <gtest/gtest.h>

#include "strategy.h"
#include "dex_transaction.h"
#include "strategy_engine.h"

using namespace algo;
using namespace std::string_literals;
using namespace std::chrono_literals;

void prepareEnvironment (algo::Engine& engine) {
	using namespace algo;
	using namespace std::string_literals;

	///filtered and referenced data
	Indicator i1(
			"base_contract",		//label
			"ETH-BTC",				//ticker
			"SellXTZBuyToken"
	);

	Indicator i2("other_contract", "XTZ-BTC", "BuyXTZSellToken");
	Indicator i3 ("quipuswap_contract", "KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1", "SellXTZBuyToken");
	engine.addTradingObject(std::move(i1));
	engine.addTradingObject(std::move(i2));
	engine.addTradingObject(std::move(i3));

	///relations of new market data and filtered/referenced data
	Signal s1(
			"basic_comparison", //label
			"Comparison",		//type of a Signal
			"LT",				//relation between the indicators
			{"base_contract", "other_contract"}, //indicators to compare
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(s1));

	Signal s2(
			"qs_vs_cb", 		//label
			"Comparison",		//type of a Signal
			"LT",				//relation between the indicators
			{"other_contract", "quipuswap_contract"}, //indicators to compare
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(s2));

	Signal s3(
			"threshhold_test", 	//label
			"Threshold",		//type of a Signal
			"GT",				//relation between the indicator and threshold
			{"other_contract", "quipuswap_contract"}, //indicators to compare
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(s3));

	Signal s4(
			"equality_test", 	//label
			"Comparison",		//type of a Signal
			"EQ",				//relation between the indicator and threshold
			{"quipuswap_contract", "quipuswap_contract"}, //indicators to compare
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(s4));


	//todo: check that this works - check relations
	///actions, based on relations
	Rule r1 (
			"enter_when_less",				//rule_label
			"KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1"s,						//ticker to generate trades for
			"SellXTZBuyToken",
			"basic_comparison",				//signal label
			"True",							//value of a signal -1, 0, 1 //todo: convert to TRUE/FALSE
			1000,
			types::Value(0.005),
			engine.getPtr<Signals>()
	);
	engine.addTradingObject(std::move(r1));

	Rule r2 (
			"enter_when_greater",			//rule_label
			"KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1"s, //ticker to generate trades for
			"BuyXTZSellToken",
//			"Exit",							//type of a Rule
			"qs_vs_cb",						//signal label
			"True",							//value of a signal -1, 0, 1 //todo: convert to TRUE/FALSE
//			"Neutral", 						//Position to enter txn
			types::Value(1000),
			types::Value(0.005),
//			"Enter", 						//Exit, Riskm Rebalance - required for prioritization if there are several Rules triggering
//			"Market",						//how to behave in the market Market/Limited/FillOrKill
			engine.getPtr<Signals>()
	);
	engine.addTradingObject(std::move(r2));

	Rule r3 (
			"enter_tokens",			//rule_label
			"KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1"s,						//ticker to generate trades for
			"SellXTZBuyToken",
			"equality_test",						//signal label
			"True",
			types::Value(1000),
			types::Value(0.005),
			engine.getPtr<Signals>()
	);
	engine.addTradingObject(std::move(r3));

	Rule r4 (
			"exit_tokens",			//rule_label
			"KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1"s,						//ticker to generate trades for
			"BuyXTZSellToken",
			"equality_test",						//signal label
			"True",
			types::Value(2000),
			types::Value(0.005),
			engine.getPtr<Signals>()
	);
	engine.addTradingObject(std::move(r4));

    Rule r5 (
            "do_nothing",			//rule_label
            "KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1"s,						//ticker to generate trades for
            "SellXTZBuyToken",
            "equality_test",						//signal label
            "False",
            types::Value(1000),
            types::Value(0.005),
            engine.getPtr<Signals>()
    );
    engine.addTradingObject(std::move(r5));


    Strategy st1 ("simple_demo 1"s, engine.getPtr<Indicators>(), engine.getPtr<Signals>(), engine.getPtr<Rules>(), 442233888);
	st1.addRule("enter_when_less");
	engine.addTradingObject(std::move(st1));

	Strategy st2 ("simple_demo 2"s, engine.getPtr<Indicators>(), engine.getPtr<Signals>(), engine.getPtr<Rules>(), 442233888);
	st2.addRule("enter_when_greater");
	engine.addTradingObject(std::move(st2));

	Strategy st3 ("tez_to_token"s, engine.getPtr<Indicators>(), engine.getPtr<Signals>(), engine.getPtr<Rules>(), 442233888);
	st3.addRule("enter_tokens");
	engine.addTradingObject(std::move(st3));

	Strategy st4 (
			"test"s,
			engine.getPtr<Indicators>(),
			engine.getPtr<Signals>(),
			engine.getPtr<Rules>(),
			442233888);
	st4.addRule("enter_tokens");
	engine.addTradingObject(std::move(st4));

    Strategy st5 ("do_nothing"s, engine.getPtr<Indicators>(), engine.getPtr<Signals>(), engine.getPtr<Rules>(), 442233888);
    st5.addRule("do_nothing");
    engine.addTradingObject(std::move(st5));

}

TEST(Strategy, LookingForGoOfflineBug) {

    TgBot::Bot bot (const_values::TG_BOT_TOKEN);
    Engine engine (&bot);
    prepareEnvironment(engine);

    ASSERT_NO_THROW(
            {
                engine.activateStrategy({"do_nothing"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
                std::this_thread::sleep_for(36000s);
            }
    );
}

/*
TEST(Strategy, SingleStrategy) {

	TgBot::Bot bot (const_values::TG_BOT_TOKEN);
	Engine engine (&bot);
	prepareEnvironment(engine);

	ASSERT_NO_THROW(
			{
				engine.activateStrategy({"simple_demo 1"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::this_thread::sleep_for(90s);
			}
	);
}

TEST(Strategy, TwoStrategies) {
	TgBot::Bot bot (const_values::TG_BOT_TOKEN);
	Engine engine (&bot);
	prepareEnvironment(engine);

	ASSERT_NO_THROW(
			{
				std::cout << "two strategies\n";
				engine.activateStrategy({"simple_demo 1"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				engine.activateStrategy({"simple_demo 2"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::this_thread::sleep_for(120s);
			}
	);

}

TEST(Strategy, AllTogether) {
	TgBot::Bot bot (const_values::TG_BOT_TOKEN);
	Engine engine (&bot);
	prepareEnvironment(engine);

	ASSERT_NO_THROW(
			{
				std::cout << "two strategies\n";
				engine.activateStrategy({"simple_demo 1"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				engine.activateStrategy({"simple_demo 2"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::this_thread::sleep_for(30s);
				std::cout << "one strategy\n";
				engine.deactivateStrategy({"simple_demo 1"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::this_thread::sleep_for(30s);
				engine.deactivateStrategy({"simple_demo 2"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::cout << "no strategy\n";
				std::this_thread::sleep_for(30s);
				std::cout << "ok\n";
				std::cout << "one strategy\n";

				std::cout << "repetitive activation\n";
				engine.activateStrategy({"simple_demo 1"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::this_thread::sleep_for(2s);
				engine.activateStrategy({"simple_demo 1"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});

				engine.activateStrategy({"simple_demo 2"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::cout << "test there and back\n";
				engine.activateStrategy({"tez_to_token"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::this_thread::sleep_for(2s);
				engine.deactivateStrategy({"tez_to_token"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::this_thread::sleep_for(90s);

				engine.activateStrategy({"test"s, "edskRqF9brudtoW87ZiRAxevLmXH1pJhQryfAwe1jjtpcSLXmcqFwcenbGFEXevXMvEYK458YocK2AyVYvBryG2CEWaY8ZNpSz"s});
				std::this_thread::sleep_for(90s);
			}
	);
}
*/