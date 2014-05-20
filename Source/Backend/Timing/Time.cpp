#include "Time.hpp"
#include "String/String.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace timing
{
	std::string get_time_string()
	{
		time_t raw_time;
		time(&raw_time);

		tm* time_info = localtime(&raw_time);

		return string::to_string(time_info->tm_hour, 1)
			 + ":" + string::to_string(time_info->tm_min, 1)
			 + ":" + string::to_string(time_info->tm_sec, 1);
	}

	std::string get_date_string()
	{
		time_t raw_time;
		time(&raw_time);

		tm* time_info = localtime(&raw_time);

		return string::to_string(1900 + time_info->tm_year)
			 + "." + string::to_string(1 + time_info->tm_mon, 1)
			 + "." + string::to_string(time_info->tm_mday, 1);
	}
}
