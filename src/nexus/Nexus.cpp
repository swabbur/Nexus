#include <nexus/Nexus.hpp>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <stdexcept>
#include <mutex>

namespace Nexus {

    static std::mutex mutex;
    static std::size_t count;

    void init() {
        std::scoped_lock<std::mutex> lock(mutex);
        if (count++ == 0) {
            WSADATA wsaData;
            int started = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (started != 0) {
                throw std::runtime_error("Could not start up Windows Sockets");
            }
        }
    }

    void shutdown() {
        std::scoped_lock<std::mutex> lock(mutex);
        if (--count == 0) {
            int cleaned = WSACleanup();
            if (cleaned != 0) {
                throw std::runtime_error("Could not clean up Windows Sockets");
            }
        }
    }

    ServerSocket open(std::uint16_t port) {

        init();

        SOCKET handle = ::socket(AF_INET, SOCK_STREAM, 0);
        if (handle == INVALID_SOCKET) {
            throw std::runtime_error("Could not create server socket");
        }

        sockaddr_in address = {};
        address.sin_family = PF_INET;
        address.sin_port = ::htons(port);

        int bound = ::bind(handle, reinterpret_cast<sockaddr *>(&address), sizeof(sockaddr_in));
        if (bound == SOCKET_ERROR) {
            ::closesocket(handle);
            throw std::runtime_error("Could not bind server socket");
        }

        int queue_size = 1;
        int listening = ::listen(handle, queue_size);
        if (listening == SOCKET_ERROR) {
            ::closesocket(handle);
            throw std::runtime_error("Could not listen on server socket");
        }

        return ServerSocket(handle);
    }

    Socket connect(std::string const & host, std::uint16_t port) {

        init();

        SOCKET handle = ::socket(AF_INET, SOCK_STREAM, 0);
        if (handle == INVALID_SOCKET) {
            throw std::runtime_error("Could not create socket");
        }

        addrinfo address_hints = {};
        address_hints.ai_family = PF_INET;
        address_hints.ai_socktype = SOCK_STREAM;
        address_hints.ai_protocol = IPPROTO_IPV4;

        addrinfo * address_info = nullptr;
        int gotten = ::getaddrinfo(host.c_str(), std::to_string(port).c_str(), &address_hints, &address_info);
        if (gotten != 0) {
            throw std::runtime_error("Could not resolve host address");
        }

        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_port = ::htons(port);
        address.sin_addr = *reinterpret_cast<in_addr *>(address_info->ai_addr);

        if (::connect(handle, reinterpret_cast<sockaddr *>(&address), sizeof(sockaddr))) {
            throw std::runtime_error("Could not connect socket to host");
        }

        return Socket(handle);
    }
}
