#include"alert_message.h"
#include <utility>
#include <iomanip>

namespace atmik
{
	namespace alerter
	{
		std::vector<alert_msg> alert_msg::create_alerts( int d, int m )
		{
			std::vector<alert_msg> alerts;

			// add_alerts<sim>(alerts);
			// we do not need to examine all sms for pending alerts
			// looking at he subset of modified sims is sufficient
			for (sim* s : sim::changed_sims)
			{
				if (!s->usage.daily.alert_sent && s->usage.daily.alert_reached())
				{
					alerts.push_back(create_daily_alert( d, m, *s));
					s->usage.daily.alert_sent = true;
				}

				if (!s->usage.monthly.alert_sent && s->usage.monthly.alert_reached())
				{
					alerts.push_back(create_monthly_alert( d, m, *s ));
					s->usage.monthly.alert_sent = true;
				}

			}

			add_alerts<group>( d, m, alerts);
			add_alerts<enterprise>( d, m, alerts);
			add_alerts<zone>( d, m, alerts);

			return alerts;
		}

		std::ostream& operator<< (std::ostream& stm, const alert_msg& am)
		{
			stm << std::setfill('0') << std::setw(2) << am.day << '/' 
				<< std::setw(2) << am.month << ' '
				<< char(am.source) << ' ' << am.sender_id << ' '
				<< char(am.type) << ' ' << am.curr_usage << ' '
				<< am.limit << ' ';

			const auto pct = am.percentage();
			if (pct <= 1000) return stm << pct << '%';
			else return stm << "greater than 10 times the limit\n";
		}

	}
}
