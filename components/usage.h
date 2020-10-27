#ifndef USAGE_H_INCLUDED
#define USAGE_H_INCLUDED

#include <string>
#include <iostream>

namespace atmik
{
	namespace alerter
	{
		struct usage_stats
		{
			unsigned long long amount = 0;
			unsigned long long limit = -1;
			unsigned int alert_percentage = 100;
			unsigned long long alert_level = -1;
			bool alert_pending = false;
			bool alert_sent = false;

			bool alert_reached() const { return amount >= alert_level; }

			std::string alert_message() const;

			void set_limits(unsigned long long limit_amt, unsigned int alert_pct);

			void add_usage(unsigned long long amt) { amount += amt; }

			void reset_usage() 
			{ 
				amount = 0; 
				alert_pending = alert_sent = false;
			}
		};

		struct usage_info
		{
			usage_stats daily;
			usage_stats monthly;

			void add_usage(unsigned long long amt)
			{
				daily.add_usage(amt);
				monthly.add_usage(amt);
			}
		};

		std::ostream& operator<< (std::ostream& stm, const usage_stats& u);
		std::ostream& operator<< (std::ostream& stm, const usage_info& u);
		std::istream& operator>> (std::istream& stm, usage_stats& u);
		std::istream& operator>> (std::istream& stm, usage_info& u);
	}
}
#endif // USAGE_H_INCLUDED