#if 1

#include "coinbase.h"
#include "tzkt_io.h"
#include "quipuswap.h"
#include "strategy.h"
#include "randomer.h"

#include "ui.h"

#include <iostream>
#include <iomanip>
#include <array>
#include <chrono>
#include <thread>

#include "bot_config.h"
#include "dex_transaction.h"
#include "strategy_engine.h"

using namespace algo;
using namespace std;
using namespace std::literals;
using namespace std::chrono_literals;
using namespace boost;
using namespace algo::data::coinbase;
using namespace algo::tezos::tzkt_io;
using namespace algo::tezos::quipuswap;
using namespace time_;

static auto const fastIO = []() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  return 0;
}();

[[maybe_unused]]
algo::Quote<types::Value, time_::Milliseconds>
        modifier (const algo::Quote<types::Value, time_::Milliseconds>& quote){
	Quote<types::Value, time_::Milliseconds> modified = quote;
	modified.value() = quote.value() - types::Value(10);
	return modified;
}


int main() {

	//todo: Check The Rules - add tickers
	//todo: check Indicator and Signal updates
	//todo: Add ThreadSafe maps
	//todo: Add Map size limit
	//todo: Add Value ctor from Json
	//todo: Add Logging at large
	//todo: Unclear what happens with a Token price - need to check that


	//todo: add a case when there is a new data for some of the labels in strategy - probably check that when ruleSignaling

#if 0
	getListOfCurrencies();
	auto r = getListOfCurrencies();
	auto t = processListOfCurrencies(std::move(r));
	Print(t, std::cout, &CurrencyData::id, &CurrencyData::name);

//	auto single_currency = processSingleCurrency(getSingleCurrency("XTZ"));
//	cout << single_currency;
//
//	single_currency = processSingleCurrency(getSingleCurrency("ETH"));
//	cout << single_currency;
//
//	single_currency = processSingleCurrency(getSingleCurrency("BTC"));
//	cout << single_currency;

#endif

#if 0
	for (int i = 0; i != 10; ++i) {
		auto trade1 = processSingleTradeData(getSingleTradeData("ETH", "USD"));
		if (not trade1.empty) {
			cout << "ETH-USD: " << trade1.price << ' ' << trade1.volume << ' ' << trade1.time << '\n';
		}
		else {
			cout << "ETH-USD: " << "no data\n";
		}

		auto trade2 = processSingleTradeData(getSingleTradeData("BTC", "USD"));
		if (not trade2.empty) {
			cout << "BTC-USD: " << trade2.price << ' ' << trade2.volume << ' ' << trade2.time << '\n';
		}
		else {
			cout << "BTC-USD: " << "no data\n";
		}

		auto trade3 = processSingleTradeData(getSingleTradeData("ETH", "BTC"));
		if (not trade3.empty) {
			cout << "ETH-BTC: " << trade3.price << ' ' << trade3.volume << ' ' << trade3.time << '\n';
		}
		else {
			cout << "ETH-BTC: " << "no data\n";
		}
	}
#endif

#if 0

//	getContractData(true, std::cout);

	QueryFactory qf;

	curl_client::Response response;
	curl_client::Request request;

	std::cout << "Storage:\n";
	auto handle = qf.getStorageQuery("KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1");
	response = request.
							  pathSetNew(std::move(handle))->
							  Implement(curl_client::Method::Get);
	Print(response, std::cout, false);
	std::cout << '\n';

	TokenPair<Seconds> token;
	token.pair_address = "KT1DksKXvCBJN7Mw6frGj6y6F3CbABWZVpj1";
	token = estimateTezToToken(std::move(token));
	std::cout << "tez pool: " << token.tez_pool << "; token_pool: " << token.token_pool << "; price: " << token.current_price << '\n';

	token = estimateTokenToTez(std::move(token));
	std::cout << "tez pool: " << token.tez_pool << "; token_pool: " << token.token_pool << "; price: " << token.current_price << '\n';

#endif

#if 0

	Engine engine;

	///filtered and referenced data
	Indicator i1(
			"ETH-BTC",				//ticker
			"base_contract"			//label
	);

	Indicator i2("ETH-USD", "other_contract");
//	strategy.addIndicator("C3", "modified_contract", modifier);
	engine.addTradingObject(std::move(i1));
	engine.addTradingObject(std::move(i2));

	///relations of new market data and filtered/referenced data
	Signal s1(
			"basic_comparison", //label
			"Comparison",		//type of a Signal
			"LT",				//relation between the indicators
			{"base_contract", "other_contract"}, //indicators to compare
			engine.getPtr<Indicators>()
	);
	engine.addTradingObject(std::move(s1));


	//todo: check that this works - check relations
	///actions, based on relations
	Rule r1 (
			"ETH-BTC"s,						//ticker to generate trades for
			"enter_when_less",				//rule_label
			"Entry",						//type of a Rule
			"basic_comparison",				//signal label
			1,								//value of a signal -1, 0, 1 //todo: convert to TRUE/FALSE
			"Neutral", 						//Position to enter txn
			types::Value(1),
			"Enter", 						//Exit, Riskm Rebalance - required for prioritization if there are several Rules triggering
			"Market",						//how to behave in the market Market/Limited/FillOrKill
			engine.getPtr<Signals>()
	);
	engine.addTradingObject(std::move(r1));

	Strategy st1 ("simple_demo"s, engine.getPtr<Indicators>(), engine.getPtr<Signals>(), engine.getPtr<Rules>());
	st1.addRule("enter_when_less");
	engine.addTradingObject(std::move(st1));

	engine.activateStrategy("simple_demo"s);

	int counter = 0;
	while (true) {
		engine.iterateOverStrategies();
		std::this_thread::sleep_for(1ms);
		cout << "==========\n";
		if (counter++ > 10) break;
	}

#endif

#if 0
	user_interface::Controller app;
	app.run();
#endif


#if 0

	auto& config = algo::config::RobotConfig::getInstance("../setup_user_data/bot_config.ini");
	config.loadFromIni();
	config.printIniFile(std::cerr);
	config.printSettings(std::cerr);

	algo::config::ContractInfo ci;
	ci.name = "name5";
	ci.ticker_cb = "CB 5";
	ci.ticker_qs = "QS 5";
	ci.price_diff = 42;
	ci.timeframe = 42;
	ci.trade_size = 42;
	ci.slippage = 4;

	config.updateIni(std::move(ci));
	config.loadFromIni();
	config.printIniFile(std::cerr);
	config.printSettings(std::cerr);

	auto c5 = config.getContractInfo("name5");
	std::cerr << c5 << '\n';

	auto fuck = config.getContractInfo("fuck");
	std::cerr << fuck << '\n';

#endif


#if 0

	using namespace types;
	using namespace algo::tezos::quipuswap::transaction;

	String result;

	os::invokeCommandAndCaptureResult(
			makeCommand("KT1EKo1Eihucz9N4cQyaDKeYRoMzTEoiZRAT").c_str(),
			result);
	std::cerr << result << '\n';

#endif




	return 0;
}
#endif

#if 0

#include <thread>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <cstring>
#include <pthread.h>
#include <unistd.h>


void show_thread(const std::string &keyword)
{
	std::string cmd("ps -T | grep ");
	cmd += keyword;
	system(cmd.c_str());
}

class Foo {
public:
	void sleep_for([[maybe_unused]] const std::string &tname, [[maybe_unused]] int num)
	{
	}

	void start_thread(const std::string &tname)
	{
		std::thread thrd = std::thread(&Foo::sleep_for, this, tname, 3600);
		tm_[tname] = thrd.native_handle();
		thrd.detach();
		std::cout << "Thread " << tname << " created:" << std::endl;
	}

	void stop_thread(const std::string &tname)
	{
		ThreadMap::const_iterator it = tm_.find(tname);
		if (it != tm_.end()) {
			pthread_cancel(it->second);
			tm_.erase(tname);
			std::cout << "Thread " << tname << " killed:" << std::endl;
		}
	}

private:
	typedef std::unordered_map<std::string, pthread_t> ThreadMap;
	ThreadMap tm_;
};



int main()
{
	Foo foo;
	std::string keyword("test_thread");
	std::string tname1 = keyword + "1";
	std::string tname2 = keyword + "2";

	// create and kill thread 1
	foo.start_thread(tname1);
	show_thread(keyword);
	foo.stop_thread(tname1);
	show_thread(keyword);

	// create and kill thread 2
	foo.start_thread(tname2);
	show_thread(keyword);
	foo.stop_thread(tname2);
	show_thread(keyword);

	return 0;
}

#endif


#if 0

#include "operators.h"

#include <boost/thread/thread.hpp>
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>
#include <map>


using namespace std::chrono_literals;
using namespace std::string_literals;

std::mutex iomutex;
void f1(int num) {
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::lock_guard<std::mutex> lk(iomutex);
	std::cout << "Thread " << num << " pthread_t " << pthread_self() << std::endl;
}

void f2(int num, int b) {
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::lock_guard<std::mutex> lk(iomutex);
	std::cout << "Thread " << num << "; second arg is " << b << " pthread_t " << pthread_self() << std::endl;
}

/// less than
template <typename L, typename R>
using HasLT = std::enable_if_t<operators::has_less_than<L, R>::value, bool>;

template <typename Label, HasLT<Label,Label> = true>
class ThreadEngine {
	using Thread = boost::thread;
	using ThreadID = boost::thread::id;
	using ThreadMap = std::map<ThreadID, Thread>;
	using ThreadMapIter = typename std::map<ThreadID, Thread>::iterator;
	using LabelIdMap = std::map<Label, ThreadID>;
	using IdLabelMap = std::map<ThreadID, Label>;
public:
	explicit ThreadEngine () = default;
	ThreadEngine (const ThreadEngine&) = delete;
	ThreadEngine operator = (const ThreadEngine&) = delete;
	void addThread (Thread&& thread, Label label){
		auto id = thread.get_id();
		thread_map.insert(std::make_pair(id, std::move(thread)));
		label_to_id.insert(std::make_pair(label, id));
		id_to_label.insert(std::make_pair(id, std::move(label)));
		thread_map.at(id).detach();
	}
	template <typename F, typename... Args>
	void createThread (Label label, F func, Args... args) {
		boost::thread _ (func, args...);
		this->addThread(std::move(_), std::move(label));
	}

	bool interruptThread (ThreadID id){
		if (auto found_thread = thread_map.find(id); found_thread != thread_map.end()) {
			__eraseThread(found_thread);
			return true;
		}
		else {
			return false;
		}
	}

	bool interruptThread (Label label){
		if (auto found_label = label_to_id.find(label); found_label != label_to_id.end()) {
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

	size_t size() const {return __size();}

private:
	ThreadMap thread_map;
	LabelIdMap label_to_id;
	IdLabelMap id_to_label;
	size_t __size() {
		if (id_to_label.size() != label_to_id.size()) {
			throw std::runtime_error("Map sizes are not equal, can't proceed.");
		}
		else {
			return id_to_label.size();
		}
	}

	void __eraseThread (ThreadMapIter found) {
		Thread tr (&ThreadEngine::__eraseThreadHelper, this, std::move(found->second));
		tr.detach();
		auto id_label = id_to_label.find(found->first);
		label_to_id.erase(id_label->second);
		id_to_label.erase(id_label);
		thread_map.erase(found);
	}
	void __eraseThreadHelper (Thread&& thread) {
		thread.interrupt();
	}


};



#include <string>

int main () {

	ThreadEngine<std::string> te;
	te.createThread("1st"s, f1, 1);
	te.createThread("2nd"s, f1, 2);
	te.createThread("3rd"s, f2, 3, 4);
	std::this_thread::sleep_for(std::chrono::seconds(5));

	return 0;
}

#endif