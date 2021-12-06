//
// Created by Andrey Solovyev on 18/11/2021.
//

#include <gtest/gtest.h>

TEST(CoinBaseAPI, Signature) {
	/*
	timestamp:  1637176695
	message:  1637176695GET/v2/user
	signature:  648a9184f6ff774ad9a3ff46ef8e953911ea6affdfda53c163c8a85810034270
		<PreparedRequest [GET]>
		{u'errors': [{u'message': u'invalid api key', u'id': u'authentication_error'}]}
	timestamp:  1637176696
	message:  1637176696POST/v2/accounts/primary/transactions{"to": "user@example.com", "amount": "10.0", "type": "send", "currency": "USD"}
	signature:  9d211b4a31a669b165cce25e06fac14151ffa4bc8365c464dea95d36c7daf8fc
*/
}