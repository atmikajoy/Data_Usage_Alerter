#include "alert_recvr.h"
#include <iostream>

namespace atmik
{
	namespace alerter
	{
		struct stdout_alert_recvr : alert_recvr
		{
			virtual void process_alerts( std::vector<alert_msg> amsgs ) override
			{
				for (const auto& am : amsgs)
				{
					std::cout << "*** alert *** " << source_name(am.source)
						<< ' ' << am << '\n';
				}
			}
		};

		static stdout_alert_recvr stdout_alert_recvr_;
	}
}
