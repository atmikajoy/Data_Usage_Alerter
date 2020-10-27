#ifndef TIME_UTILS_H_INCLUDED
#define TIME_UTILS_H_INCLUDED

#include <ctime>
#include <string>

namespace atmik
{
	namespace util
	{
		std::string time_stamp(std::time_t t);
		std::string time_stamp(const std::tm& start, int elapsed_seconds);
		std::time_t to_time_t(const std::string& time_stamp);
		std::tm to_tm(const std::string& time_stamp);

		inline int extract(const std::string& time_stamp, std::size_t pos, std::size_t nchars)
		{
			return std::stoi(time_stamp.substr(pos, nchars));
		}

		// YYYY-MM-DD hh:mm:ss
		// 0123456789012345678
		inline int year(const std::string& time_stamp) { return extract(time_stamp, 0, 4); }
		inline int month(const std::string& time_stamp) { return extract(time_stamp, 5, 2); }
		inline int day(const std::string& time_stamp) { return extract(time_stamp, 8, 2); }
		inline int hour(const std::string& time_stamp) { return extract(time_stamp, 11, 2); }
		inline int min(const std::string& time_stamp) { return extract(time_stamp, 14, 2); }
		inline int sec(const std::string& time_stamp) { return extract(time_stamp, 17, 2); }
	}
}


#endif // TIME_UTILS_H_INCLUDED
