#include "alert_sender.h"
#include <sstream>
#include"../util/sock_util.h"
#include <thread>
#include <atomic>
#include <chrono>
namespace atmik
{
	namespace alerter
	{
		namespace nw
		{
			std::string to_string(const alert_msg& am)
			{
				std::ostringstream stm;
				stm << am;
				return stm.str();
			}

			alert_msg from_string(const std::string& str)
			{
				char source;
				std::string id;
				char type;
				unsigned long long curr_usage;
				unsigned long long limit;

				std::istringstream stm(str);
				if (stm >> source >> id >> type >> curr_usage >> limit)
				{
					return alert_msg(alert_msg::source_t(source), id,
						alert_msg::alert_t(type), curr_usage, limit);
				}

				throw std::invalid_argument("bad string for alert mesage");
			}

			std::string str_sock_error(int ec)
			{
				char buffer[4096];
				if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ec, 0, buffer, sizeof(buffer), nullptr) > 0)
					return buffer;
				else return "error: " + std::to_string(ec);
			}

			void perror()
			{
				std::cerr << " *** error *** " << str_sock_error() << '\n';
			}

			std::atomic<int> num_clients(0);
			std::atomic<bool> quit_flag(false);


			void nw::service_client(int connected_socket, std::vector<alert_msg> alerts)
			{
				std::cout << "client connected, num_clients == " << num_clients << '\n';
				++num_clients;
				std::string str;
				for (const auto& a : alerts)
				{
					str = convert_str(a);
					recv_string(connected_socket, str);
					std::cout << ".";
					if (str == HUP)
					{
						std::cout << "HUP recd, closing connection\n";

						break;
					}
					send_string(connected_socket, { str.begin(), str.end() });
				}


				closesocket(connected_socket);
				--num_clients;
				std::cout << "client connected, finish thread function. num_clients == "
					<< num_clients << '\n';
			}

			void nw::quit_monitor()
			{
				std::cout << "\n==================================================================\n"
					"*********** enter 'quit' at any time to gracefully shut down **********\n"
					"==================================================================\n";

				std::string str;
				while (std::getline(std::cin, str)) if (str == "quit") quit_flag = true;

				quit_flag = true;
			}

			void nw::listen_thread(int sock)
			{
				while (!quit_flag)
				{
					int result = listen(sock, 1);
					if (result == SOCKET_ERROR)
					{
						perror();
						return;
					}
					else std::cout << "listening for an incomming connection\n";

					int connected_socket = accept(sock, nullptr, nullptr);
					if (connected_socket == INVALID_SOCKET)
					{
						perror();
						return;
					}

					std::cout << "got a connection socket == " << connected_socket << '\n';

					std::thread( service_client, connected_socket, 
						          std::vector<alert_msg>{} ).detach();
				}
			}

			int nw::socket_func()
			{
				WSADATA wsad;
				if (WSAStartup(MAKEWORD(2, 0), &wsad) != 0)
				{
					std::cout << "WSAStartup failed\n";
					return 1;
				}

				// connect to cppreference.com over http  (176.89.34.4 on port 12345)

				addrinfo hints{};
				hints.ai_family = AF_INET;
				hints.ai_socktype = SOCK_STREAM; // tcp/ip
				hints.ai_flags = AI_PASSIVE;

				addrinfo* server_address;
				int result = getaddrinfo(nullptr, service.c_str(), &hints, &server_address);

				if (result != 0)
				{
					std::cerr << "getaddrinfo failed: " << gai_strerror(result) << '\n';
					return 1;
				}

				const sockaddr_in* sin = reinterpret_cast<const sockaddr_in*>(server_address->ai_addr);
				const void* address = &sin->sin_addr;
				char cstr[1024]{};
				inet_ntop(AF_INET, address, cstr, sizeof(cstr));
				const int port = ntohs(sin->sin_port);
				std::cout << "IPV4: " << cstr << "  port: " << port << '\n';

				int bind_socket = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);

				if (bind_socket == INVALID_SOCKET)
				{
					perror();
					return 1;
				}

				result = bind(bind_socket, server_address->ai_addr, server_address->ai_addrlen);

				if (result == SOCKET_ERROR)
				{
					perror();
					return 1;
				}
				else std::cout << "bound to the end point\n";

				/*/////////////////// repeat for each connection //////////////////////////*/

				std::thread(quit_monitor).detach();

				std::thread(listen_thread, bind_socket).detach();

				using namespace std::chrono_literals;
				while (!quit_flag) std::this_thread::sleep_for(500ms);
				/*//////////////////////////////////////////////////////////////////*/

				if (num_clients) std::cout << "quit flag set. waiting for " <<num_clients << " clients to disconnect\n";
				using namespace std::chrono_literals;
				while (num_clients > 0)
				{
					std::this_thread::sleep_for(2s);
					std::cout << '.' << std::flush;
				}
				std::cout << "\ndone\n";

				freeaddrinfo(server_address);
				closesocket(bind_socket);
				// windows specific
				WSACleanup();
			}

			void nw::send_alerts(int socket)
			{
				auto alerts = alert_msg::create_alerts();
				socket_func();
				service_client(socket, alerts);
				// TO DO: send the alerts in he vector
				// this is just a stub for now
				if (!alerts.empty())
				{
					std::cout << "*** " << alerts.size() << "  *** alerts\n" << std::flush;
				}

				for (const atmik::alerter::alert_msg& a : alerts)
					std::cout << "alert: " << a << '\n' << std::flush;
			}
		}
	}
}