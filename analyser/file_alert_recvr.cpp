#include "alert_recvr.h"
#include <fstream>
#include "../util/time_utils.h"
// #include <iostream>

namespace atmik
{
	namespace alerter
	{
		namespace
		{
			const char alert_dir[] = "C:/cdr/alerts/";

			std::string simple_time_stamp()
			{
				std::string str = atmik::util::time_stamp(std::time(nullptr));
				std::string ts;
				for (unsigned char c : str)
					if (std::isalnum(c)) ts += char(c);
				return ts;
			}
		}

		// there is no header for this class
		// an instance with static storage duration in instantiated
		// the base class constructor adds (a pointer to) it to the set of recvrsa
		// overridden process_alerts is called (polymorphically) on this object
		struct file_alert_recvr : alert_recvr
		{
			virtual void process_alerts(std::vector<alert_msg> amsgs) override
			{
				const auto ts = simple_time_stamp();
				const std::string path = alert_dir + ts + ".alerts";
				// std::cout << "alert file path: " << path << '\n';
				if( std::ofstream file{ path , std::ios::out|std::ios::app })
				{
					for (const auto& am : amsgs)
					{
						file << "*** alert *** " << source_name(am.source)
							<< ' ' << am << '\n';
					}
				}
			}
		};

		static file_alert_recvr file_alert_recvr_;
	}
}
