//
// Created by Andrey Solovyev on 02/11/2021.
//

#include "timestamp.h"

namespace algo {
  namespace timestamp {

	types::String makeDateTimeString(time_t t, bool isUTC) {

		struct tm s_tm;
		if (isUTC) {
			gmtime_r(&t, &s_tm);
		} else {
			localtime_r(&t, &s_tm);
		}

#ifdef _WIN32
		wchar_t buff[100];
  	std::wcsftime(buff, 100, L"%Y%m%d %H:%M:%S", &s_tm);
#else
		static char buff[40];
		strftime(buff, 40, "%Y%m%d %H:%M:%S", &s_tm);
#endif

		return types::String(buff);
	}

	timestamp::Timestamp::Timestamp()
			: value(makeDateTimeString(std::time(nullptr), true))
	{}


//  timestamp::Timestamp::Timestamp(const types::String& value)
//		  :value(value) { }

	bool operator==(const Timestamp& lhs, const Timestamp& rhs){
		auto res = std::strcmp(lhs.value.c_str(), rhs.value.c_str());
		return res == 0;
	}
	bool operator!=(const Timestamp& lhs, const Timestamp& rhs){
		return not (rhs==lhs);
	}
	bool operator < (const Timestamp& lhs, const Timestamp& rhs){
		auto res = std::strcmp(lhs.value.c_str(), rhs.value.c_str());
		return res < 0;
	}
	bool operator >(const Timestamp& lhs, const Timestamp& rhs){
		return not (rhs < lhs);
	}
	bool operator<=(const Timestamp& lhs, const Timestamp& rhs){
		return not (rhs > lhs);
	}
	bool operator>=(const Timestamp& lhs, const Timestamp& rhs) {
		return not (lhs<rhs);
	}

	std::ostream& operator<<(std::ostream& os, const Timestamp& timestamp){
		os << timestamp.value;
		return os;
	}


  }//!namespace
}//!namespace

