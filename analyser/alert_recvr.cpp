#include "alert_recvr.h"
#include <thread>
#include "../util/threadpool.h"

namespace atmik
{
	namespace alerter
	{
		std::set< alert_recvr* >& alert_recvr::recvrs()
		{
			static std::set< alert_recvr* > recvrs_;
			return recvrs_;
		}

		alert_recvr::alert_recvr()
		{
			recvrs().insert(this);
		}

		alert_recvr::~alert_recvr()
		{
			recvrs().erase(this);
		}

		void alert_recvr::send_alerts(const std::vector<alert_msg>& amsgs)
		{
			static auto& pool = atmik::util::thread_pool::default_pool();
			static auto pfn_process_alerts = &alert_recvr::process_alerts;

			for (alert_recvr* ptr_object : recvrs())
			{
				// we need to make a copy of the messages for use by the thread
				auto amsgs_cpy = amsgs;
				pool.queue_task( pfn_process_alerts, ptr_object, std::move(amsgs_cpy) );
			}
		}
	}
}
