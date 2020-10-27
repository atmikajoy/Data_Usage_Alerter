#include "alert_recvr.h"
#include <set>
#include "../util/sock_util.h"
#include <mutex>
#include "../util/config.h"

// #define UNIT_TEST // for testing, comment this out later

namespace atmik
{
	namespace alerter
	{
		// there is no header for this class
		// an instance with static storage duration in instantiated
		// the base class constructor adds (a pointer to) it to the set of recvrsa
		// overridden process_alerts is called (polymorphically) on this object

		struct nw_alert_recvr : alert_recvr
		{
			// each client is identified by our end of a connected socket
			static void add_client(int socket_fd)
			{
				std::lock_guard lock(mutex());
				connected_sockets().insert(socket_fd);
			}

			// send the alert to each of the connected clients
			// nw_alert_recvr::listener listener runs in a backgrounf thread 
			// and calls this function when a connection is accepted
			virtual void process_alerts(std::vector<alert_msg> amsgs) override
			{
				const auto str_msgs = to_string(amsgs);
				for ( std::string str : str_msgs )
				{
					std::lock_guard lock( mutex() );

					// we can't use a range-based for loop here because 
					// we may need to erase an item in the middle of the loop
					for ( auto iter = connected_sockets().begin() ; 
						  iter != connected_sockets().end() ; )
					{
						int sock = *iter;
						if (send_string(sock, str) != 1)
						{
							// send_string failed, this client is no longer connected
							// erase the item and update iter to 'point' yo the item
							// ommediately after the erased item
							iter = connected_sockets().erase(iter);
						}

						else
							++iter; 
					}
				}
			}

			static std::set<int>& connected_sockets() ;
			static std::mutex& mutex();
			static std::string  to_string(const alert_msg& am);
			static std::vector<std::string> to_string(const std::vector<alert_msg>& amsgs);
		    
			static const std::string& port() ;
			static int do_bind();
			static int bind_socket;
			static void listener();
			static void recvr( int sock ); // used for testing
		};
		
		static nw_alert_recvr nw_alert_recvr_;

		int nw_alert_recvr::bind_socket = do_bind();

		namespace
		{
			// looks like an an output stream, but it does nothing
			struct null_ostream 
			{
				// note: this will not work with (templated) i/o manipulators
				template < typename T > 
				const null_ostream& operator<< (T&&) const noexcept 
				{ return *this; }

				// TO DO: add manipulator support (if required)
			};

            #ifdef NDEBUG
			     // not debugging, disable debug output
			     const null_ostream debug_out;
            #else
			     // debug, output debug messages to std::clog
			     std::ostream& debug_out = std::clog;
            #endif // NDEBUG
		}

		void nw_alert_recvr::listener()
		{
			while (true)
			{
				int result = listen(bind_socket, 5);
				debug_out << "listen returned " << result << '\n';
				if (result == SOCKET_ERROR) break;

				debug_out << "calling accept\n";
				int sock = accept(bind_socket, nullptr, nullptr);
				debug_out << "accept returned " << sock << '\n';

				if (sock == INVALID_SOCKET)
				{
					debug_out << "error code: " << WSAGetLastError() << '\n' ;
					continue;
				}
				
				else add_client(sock);
			}
		}

		void nw_alert_recvr::recvr( int sock )
		{
			std::string str;
			while (true)
			{
				if (recv_string(sock, str) != 1) break;
				std::cout << "*** recd *** " << str << '\n';
			}
		}

		std::set<int>& nw_alert_recvr::connected_sockets()
		{
			static std::set<int> connected_sockets_;
			return connected_sockets_;
		}

		std::mutex& nw_alert_recvr::mutex()
		{
			static std::mutex mutex_;
			return mutex_;
		}

		std::string  nw_alert_recvr::to_string(const alert_msg& am)
		{
			std::ostringstream stm;
			stm << am;
			return stm.str();
		}

		std::vector<std::string> nw_alert_recvr::to_string(const std::vector<alert_msg>& amsgs)
		{
			std::vector<std::string> strings;
			for (const auto& am : amsgs) strings.push_back( to_string(am) );
			return strings;
		}

		const std::string& nw_alert_recvr::port()
		{
			static const std::string port_ = 
				atmik::config::get<std::string>( "alerter.alert_port", "12345" ) ;
			
			return port_;
		}

		int nw_alert_recvr::do_bind()
		{
			WSADATA wsad;
			if (WSAStartup(MAKEWORD(2, 0), &wsad) != 0) return 1;

			addrinfo hints{};
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM; // tcp/ip
			hints.ai_flags = AI_PASSIVE;

			addrinfo* server_address;
			int result = getaddrinfo(nullptr, port().c_str(), &hints, &server_address);

			if (result != 0) return 1;

			const sockaddr_in* sin = reinterpret_cast<const sockaddr_in*>(server_address->ai_addr);
			const void* address = &sin->sin_addr;
			char cstr[1024]{};
			inet_ntop(AF_INET, address, cstr, sizeof(cstr));
			const int port = ntohs(sin->sin_port);
			std::cout << "IPV4: " << cstr << "  port: " << port << '\n';

			bind_socket = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);

			if (bind_socket == INVALID_SOCKET) return INVALID_SOCKET;

			result = ::bind(bind_socket, server_address->ai_addr, server_address->ai_addrlen);

			if (result == SOCKET_ERROR) 
			{
				closesocket(bind_socket);
				bind_socket = INVALID_SOCKET;
			}

			else
			{
				// start a background listener thread
				std::thread(nw_alert_recvr::listener).detach();
			}
			
			#ifdef UNIT_TEST
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				char buffer[128];
				inet_ntop(server_address->ai_family, address, buffer, 128);
				int test_sock = socket(server_address->ai_family, 
					       server_address->ai_socktype, server_address->ai_protocol);
				int result = connect( test_sock, (const sockaddr*)&sin->sin_addr, server_address->ai_addrlen);
				auto ec = WSAGetLastError();
				std::cout << "connect returned " << result << '\n'
					      << "error code: " << ec << '\n';
				std::thread(&nw_alert_recvr::recvr, test_sock).detach();
			}

			#endif // UNIT_TEST

			return bind_socket;
		}
	}
}
