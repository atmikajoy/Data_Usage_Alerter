#ifndef SOCK_UTIL_H_INCLUDED
#define SOCK_UTIL_H_INCLUDED

#define NOMINMAX
#include <iostream>
#include <string>
#undef UNICODE
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment( lib, "ws2_32.lib")

const std::string service = "12345";
// const std::size_t data_sz = 100;
const std::size_t PREFIX_SIZE = 12;
const std::string HUP = "BYE";

inline std::string add_length_prefix(const std::string& str)
{
    std::string prefix = std::to_string(str.size());
    prefix.resize(PREFIX_SIZE, ' ');
    return prefix + str;
}

// return -1 on failure
// return 0 if connection is closed
// return 1 on success
inline int send_string(int sock, const std::string& str)
{
    const auto pstr = add_length_prefix(str);
    std::size_t nbytes_sent = 0;
    while (nbytes_sent < pstr.size())
    {
        int res = send(sock, &pstr.front() + nbytes_sent, pstr.size() - nbytes_sent, 0);
        if (res < 1) return res;
        else nbytes_sent += res;
    }
    return 1;
}

// return -1 on failure
// return 0 if connection is closed
// return 1 on success
inline int recv_raw_string(int sock, std::string& str, std::size_t nbytes)
{
    str.clear();
    str.resize(nbytes, ' ');
    std::size_t bytes_recd = 0;
    while (bytes_recd < nbytes)
    {
        int res = recv(sock, &str.front() + bytes_recd, nbytes - bytes_recd, 0);
        if (res < 1) return res;
        else bytes_recd -= res;
    }
    return 1;
}

// return -1 on failure
// return 0 if connection is closed
// return 1 on success
// return -2 on bad prefix
inline int recv_string(int sock, std::string& str)
{
    std::string prefix;
    int res = recv_raw_string(sock, prefix, PREFIX_SIZE);
    if (res < 1) return res;
    std::size_t nbytes = 0;
    try { nbytes = std::stoul(prefix); }
    catch (const std::exception&) { return -2; }

    return recv_raw_string(sock, str, nbytes);
}

#endif // SOCK_UTIL_H_INCLUDED
