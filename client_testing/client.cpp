#include "../util/sock_util.h"
#include <iomanip>

std::string str_sock_error(int ec = WSAGetLastError())
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

int main()
{
	// windows specific
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
	int result = getaddrinfo("127.0.0.1", service.c_str(), &hints, &server_address);

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

	int sock = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);

	if (sock == INVALID_SOCKET)
	{
		perror();
		return 1;
	}

	std::cout << "try to connect to server\n";

	result = connect(sock, server_address->ai_addr, server_address->ai_addrlen);

	if (result == SOCKET_ERROR)
	{
		perror();
		return 1;
	}

	else std::cout << "connected to server\n";

	std::string msg;
	while (recv_string(sock, msg) == 1)
		std::cout << "recd string: " << std::quoted(msg) << '\n';

	closesocket(sock);
	/*//////////////////////////////////////////////////////////////////*/

	freeaddrinfo(server_address);
	// windows specific
	WSACleanup();
}
