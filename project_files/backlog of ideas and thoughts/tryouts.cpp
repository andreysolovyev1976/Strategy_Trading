//
// Created by Andrey Solovyev on 15/11/2021.
//

#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <iostream>
#include <iomanip>



// Convert string of chars to its representative string of hex numbers
void stream2hex(const std::string str, std::string& hexstr, bool capital = false)
{
	hexstr.resize(str.size() * 2);
	const size_t a = capital ? 'A' - 1 : 'a' - 1;

	for (size_t i = 0, c = str[0] & 0xFF; i < hexstr.size(); c = str[i / 2] & 0xFF)
	{
		hexstr[i++] = c > 0x9F ? (c / 16 - 9) | a : c / 16 | '0';
		hexstr[i++] = (c & 0xF) > 9 ? (c % 16 - 9) | a : c % 16 | '0';
	}
}

// Convert string of hex numbers to its equivalent char-stream
void hex2stream(const std::string hexstr, std::string& str)
{
	str.resize((hexstr.size() + 1) / 2);

	for (size_t i = 0, j = 0; i < str.size(); i++, j++)
	{
		str[i] = (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) << 4, j++;
		str[i] |= (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) & 0xF;
	}
}



#include "curl_client.h"
using namespace curl_client;

#include "timestamp.h"
using namespace algo::timestamp;

#include <chrono>


namespace types {
  using byte = uint8_t;
  using bytes = std::vector<byte>;
  using String = std::string;
}

namespace encoding {
  namespace hex {

	types::String Convert(const types::bytes& bytes)
	{
		int size = bytes.size();
		types::String chars;
		chars.resize(size << 1);

		for (int i = 0, j = 0; i<size; ++i) {
			types::byte l = bytes[i] & 15;
			types::byte h = bytes[i] >> 4;

			chars[j++] = (char) (h+(h>9 ? 87 : 48));
			chars[j++] = (char) (l+(l>9 ? 87 : 48));
		}

		return chars;
	}

	template<typename I>
	std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1)
	{
		static const char* digits = "0123456789ABCDEF";
		std::string rc(hex_len, '0');
		for (size_t i = 0, j = (hex_len-1)*4; i<hex_len; ++i, j -= 4)
			rc[i] = digits[(w >> j) & 0x0f];
		return rc;
	}

  }//!namespace
}//!namespace


#include <openssl/hmac.h>

types::String GetMsgSignature (const types::String &secret_key, const types::String &message) {

//	char *key = strdup(secret_key.c_str());
	const char *key = secret_key.c_str();
	const unsigned char *data = (const unsigned char *)strdup(message.c_str());

	int
			keylen = secret_key.size(),
			datalen = message.size();

	unsigned char *result = NULL;
	unsigned int resultlen = -1;

	result = HMAC(EVP_sha256(), (const void *)key, keylen, data, datalen, result, &resultlen);

	//todo: remove conversion
	types::bytes hex_bytes;
	hex_bytes.reserve(resultlen);
	for (unsigned int i = 0; i != resultlen; i++){
		hex_bytes.emplace_back(static_cast<types::byte>(result[i]));
	}
	return encoding::hex::Convert(hex_bytes);
}

#include "signalrclient/http_client.h"
#include "signalrclient/scheduler.h"

using namespace signalr;

class test_http_client : public http_client
{
public:
	test_http_client(std::function<http_response(const std::string& url, http_request request, cancellation_token token)> create_http_response_fn);
	void send(const std::string& url, http_request& request,
			std::function<void(const http_response&, std::exception_ptr)> callback, cancellation_token token) override;

	void set_scheduler(std::shared_ptr<scheduler> scheduler);
private:
	std::function<http_response(const std::string& url, http_request request, cancellation_token token)> m_http_response;

	std::shared_ptr<scheduler> m_scheduler;
};

test_http_client::test_http_client(std::function<http_response(const std::string& url, http_request request, cancellation_token token)> create_http_response_fn)
		: m_http_response(create_http_response_fn)
{
}

void test_http_client::send(const std::string& url, http_request& request,
		std::function<void(const http_response&, std::exception_ptr)> callback, cancellation_token token)
{
	auto create_response = m_http_response;
	m_scheduler->schedule([create_response, url, request, callback, token]()
	{
	  http_response response;
	  std::exception_ptr exception;
	  try
	  {
		  response = create_response(url, request, token);
	  }
	  catch (...)
	  {
		  exception = std::current_exception();
	  }

	  callback(std::move(response), exception);
	});
}

void test_http_client::set_scheduler(std::shared_ptr<scheduler> scheduler)
{
	m_scheduler = scheduler;
}



int main () {

/*
API_KEY = 'API_KEY'
API_SECRET = 'API_SECRET'

timestamp:  1637176695
message:  1637176695GET/v2/user
signature:  648a9184f6ff774ad9a3ff46ef8e953911ea6affdfda53c163c8a85810034270
<PreparedRequest [GET]>
{u'errors': [{u'message': u'invalid api key', u'id': u'authentication_error'}]}
timestamp:  1637176696
message:  1637176696POST/v2/accounts/primary/transactions{"to": "user@example.com", "amount": "10.0", "type": "send", "currency": "USD"}
signature:  9d211b4a31a669b165cce25e06fac14151ffa4bc8365c464dea95d36c7daf8fc
 *
 */
	/*

	std::string secs = "1637176695";
//	auto message = secs + "GET" + "/v2/user";// + (request.body or '')
//	std::string message = R"(1637176696POST/v2/accounts/primary/transactions{"to": "user@example.com", "amount": "10.0", "type": "send", "currency": "USD"})";
	auto signature = GetMsgSignature("API_SECRET", message);

	std::cerr << signature << '\n';
	std::cout << "648a9184f6ff774ad9a3ff46ef8e953911ea6affdfda53c163c8a85810034270" << '\n';
	std::cout << "9d211b4a31a669b165cce25e06fac14151ffa4bc8365c464dea95d36c7daf8fc" << '\n';
*/


	///coinbase is a fiat exchange
	{
		Response response;
		Request request;

		//get currencies
		response = request.
								  pathSetNew("https://api.coinbase.com/v2/currencies")->
								  Implement(Method::Get);
		std::cout << "currencies: ";
		Print(response, std::cout);
		std::cout << '\n';


		//get exchange
		response = request.
//							  pathSetNew("https://api.coinbase.com/v2/exchange-rates?currency=BTC")->
								  pathSetNew("https://api.coinbase.com/v2/exchange-rates")->
								  Implement(Method::Get);
		std::cout << "exchange: ";
		Print(response, std::cout);
		std::cout << '\n';


		//get sell
		response = request.
								  pathSetNew("https://api.coinbase.com/v2/prices/BTC-USD/sell")->
								  Implement(Method::Get);
		std::cout << "sell: ";
		Print(response, std::cout);
		std::cout << '\n';

		//get buy
		response = request.
								  pathSetNew("https://api.coinbase.com/v2/prices/BTC-USD/buy")->
								  Implement(Method::Get);
		std::cout << "buy: ";
		Print(response, std::cout);
		std::cout << '\n';


		//get spot
		response = request.
								  pathSetNew("https://api.coinbase.com/v2/prices/BTC-USD/spot")->
								  Implement(Method::Get);
		std::cout << "spot: ";
		Print(response, std::cout);
		std::cout << '\n';

		response = request.
								  pathSetNew("https://api.coinbase.com/v2/time")->
								  Implement(Method::Get);
		std::cout << "coinbase timestamp: ";
		Print(response, std::cout);
		std::cout << '\n';

		/*
		Timestamp<Sec> timestamp;
		std::string secs = std::to_string(timestamp.time_point.time_since_epoch().count());
		auto message = secs+"GET"+"/v2/user";// + (request.body or '')
		auto api_secret("...");
		auto api_key("...");
		auto signature = GetMsgSignature(api_secret, message);

		response = request.
								  SetVerbose(false)->
								  pathSetNew("https://api.coinbase.com/v2/user")->
								  headerSetSingle(HttpHeader{"CB-ACCESS-SIGN", signature})->
								  headerSetSingle(HttpHeader{"CB-ACCESS-TIMESTAMP", secs})->
								  headerSetSingle(HttpHeader{"CB-ACCESS-KEY", api_key})->

								  Implement(Method::Get);

		Print(response, std::cout);
		 */
	}

	///exchange.coinbase is a dex
	{
		Response response;
		Request request;

		//get currencies
		response = request.
								  pathSetNew("https://api.exchange.coinbase.com/currencies")->
								  Implement(Method::Get);
		std::cout << "currencies: ";
		Print(response, std::cout);
		std::cout << '\n';

		//get currencies
		response = request.
								  pathSetNew("https://api.exchange.coinbase.com/currencies/XTZ")->
								  Implement(Method::Get);
		std::cout << "currency ID: ";
		Print(response, std::cout);
		std::cout << '\n';


		//get all products
//		response = request.
//								  pathSetNew("https://api.exchange.coinbase.com/products")->
//								  Implement(Method::Get);
//		std::cout << "products: ";
//		Print(response, std::cout);
//		std::cout << '\n';
//
//
//		//get single product
		response = request.
								  pathSetNew("https://api.exchange.coinbase.com/products/XTZ-USD")->
								  Implement(Method::Get);
		std::cout << "single product: ";
		Print(response, std::cout);
		std::cout << '\n';
//
//
//
//		//get product book with a level
				response = request.
										  pathSetNew("https://api.exchange.coinbase.com/products/WCFG-USD/book")->
										  Implement(Method::Get);
		std::cout << "product book ";
		Print(response, std::cout);
		std::cout << '\n';


		//get ticker
		//Gets snapshot information about the last trade (tick), best bid/ask and 24h volume.
		response = request.
								  pathSetNew("https://api.exchange.coinbase.com/products/WCFG-USD/ticker")->
								  Implement(Method::Get);
		std::cout << "ticker product: ";
		Print(response, std::cout);
		std::cout <<'\n';
		/// 0.359 ms is a round trip

		//get product trades
		//Gets a list the latest trades for a product.
		//todo: add query data
		response = request.
								  pathSetNew("https://api.exchange.coinbase.com/products/XTZ-USD/trades")->
								  Implement(Method::Get);
		std::cout << "trades product: ";
		Print(response, std::cout);
		std::cout << '\n';

	}


//	std::vector<std::string> headers;
//	headers.emplace_back("");
//	curl->setHeaders();
//
//	Response response;
//	Factory factory;
//	factory.pathPushBackElement("https://api.coinbase.com/v2/user");
//	response = factory.
//							  requestSetVerbose(false)->
//							  requestImplement(Method::Get);
//	Print (response, std::cout, true);


	/*
   Generated a new one:
   Passphrase: 23yo7yf078q
   API Secret: ...
   API Key: ...

	*/

	return 0;
}
