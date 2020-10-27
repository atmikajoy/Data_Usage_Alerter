#include"alert_message.h"
#include <utility>
#include <set>
#include <vector>

namespace atmik
{
	namespace alerter
	{
		struct alert_recvr
		{
			virtual ~alert_recvr();

			// function called when alerts are available for processinbg
			// derived classes must override and implement this function
			virtual void process_alerts( std::vector<alert_msg> ) = 0;

			// iterate through the set of recvrs 
			// (objects of classes derived from this class).
			// call process_alerts() for each of those
			// process_alerts() of alert_recvrs is called in parallel, 
			// in background threads from the fefao;t thread pool
			static void send_alerts(const std::vector<alert_msg>&);

		    protected:
				alert_recvr();

		    private:

				// set of all objects of classes derived from this class
				// the base class constructor adds an entry into this set,
				//             the destructor erases the entry 
			    static std::set< alert_recvr*>& recvrs() ;
                
				// non-copyable, nom-assignable
				alert_recvr(const alert_recvr&) = delete;
				alert_recvr(alert_recvr&&) = delete;
				alert_recvr& operator= (const alert_recvr&) = delete;
				alert_recvr& operator= (alert_recvr&&) = delete;
		};
	}
}
