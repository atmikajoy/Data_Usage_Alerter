#ifndef ALERT_SENDER_H_INCLUDED
#define ALERT_SENDER_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#undef UNICODE
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "alert_message.h"
#include <string>

namespace atmik
{
	namespace alerter
	{
		namespace nw
		{

			std::string str_sock_error(int ec = WSAGetLastError());
			void service_client(int connected_socket, std::vector<alert_msg> alerts = {});
			void quit_monitor();
			void listen_thread(int sock);
			int socket_func();

			void send_alerts(int socket);

			std::string to_string(const alert_msg& am);

			alert_msg from_string(const std::string& str);
		}
	}
}

#endif // ALERT_SENDER_H_INCLUDED
