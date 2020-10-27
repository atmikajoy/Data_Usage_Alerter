#include "usage.h"
#include <cmath>

namespace atmik
{
	namespace alerter
	{
		namespace
		{
			unsigned long long round(double v)
			{
				return (unsigned long long) std::round(v);
			}
		}

		void usage_stats::set_limits(unsigned long long limit_amt, unsigned int alert_pct)
		{
			limit = limit_amt;
			alert_percentage = alert_pct;
			alert_level = alert_percentage > 0 ? 
				   round(limit * (alert_percentage / 100.0)) : -1 ;
		}

		std::string usage_stats::alert_message() const
		{ 
			if (!alert_reached()) return {};

			const auto pct = std::to_string( round(amount * 100.0 / limit) );
			return "usage: " + std::to_string(amount) + " (" + pct + "%)"
				"  limit: " + std::to_string(limit) +
				"  alert percentage: " + std::to_string(alert_percentage);
		}

		std::ostream& operator<< (std::ostream& stm, const usage_stats& u)
		{
			return stm << u.amount << ' ' << u.limit << ' '
				<< u.alert_percentage << ' ' << u.alert_pending
				<< ' ' << u.alert_sent;
		}

		std::ostream& operator<< (std::ostream& stm, const usage_info& u)
		{
			return stm << u.daily << ' ' << u.monthly;
		}

		std::istream& operator>> (std::istream& stm, usage_stats& u)
		{
			if (stm >> u.amount >> u.limit
				>> u.alert_percentage >> u.alert_pending
				>> u.alert_sent) u.set_limits(u.limit, u.alert_percentage);
			return stm;
		}

		std::istream& operator>> (std::istream& stm, usage_info& u)
		{
			return stm >> u.daily >> u.monthly;
		}

	}
}
