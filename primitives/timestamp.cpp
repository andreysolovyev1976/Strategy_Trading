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

	timestamp::Timestamp::Timestamp(types::String value)
			:value(std::move(value)) { }

//  timestamp::Timestamp::Timestamp(const types::String& value)
//		  :value(value) { }

	bool operator==(const Timestamp& lhs, const Timestamp& rhs){
		return lhs.value==rhs.value;
	}
	bool operator!=(const Timestamp& lhs, const Timestamp& rhs){
		return !(rhs==lhs);
	}
	bool operator <(const Timestamp& lhs, const Timestamp& rhs){
		return lhs.value<rhs.value;
	}
	bool operator >(const Timestamp& lhs, const Timestamp& rhs){
		return rhs<lhs;
	}
	bool operator<=(const Timestamp& lhs, const Timestamp& rhs){
		return !(rhs<lhs);
	}
	bool operator>=(const Timestamp& lhs, const Timestamp& rhs) {
		return !(lhs<rhs);
	}

	std::ostream& operator<<(std::ostream& os, const Timestamp& timestamp){
		os << timestamp.value;
		return os;
	}


  }//!namespace
}//!namespace

