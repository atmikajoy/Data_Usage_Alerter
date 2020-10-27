#define _CRT_SECURE_NO_WARNINGS
#include "time_utils.h"

namespace atmik
{
	namespace util
	{
		std::string time_stamp(std::time_t t)
		{
			const std::tm* ptm = std::localtime(std::addressof(t));
			char cstr[128];
			std::strftime(cstr, sizeof(cstr), "%Y-%m-%d %H:%M:%S", ptm);
			return cstr;
		}

		std::string time_stamp(const std::tm& start, int elapsed_seconds)
		{
			auto adjusted_tm = start;
			adjusted_tm.tm_sec += elapsed_seconds;
			return time_stamp(std::mktime(std::addressof(adjusted_tm)));
		}

		std::time_t to_time_t(const std::string& time_stamp)
		{
			std::tm tm = to_tm(time_stamp);
			return std::mktime(std::addressof(tm));
		}

		namespace
		{
		}

		std::tm to_tm(const std::string& time_stamp)
		{
			std::tm tm{};
			tm.tm_year = year(time_stamp) - 1900;
			tm.tm_mon = month(time_stamp) - 1;
			tm.tm_mday = day(time_stamp);
			tm.tm_hour = hour(time_stamp);
			tm.tm_min = min(time_stamp);
			tm.tm_sec = sec(time_stamp);
			return tm;
		}
	}
}
