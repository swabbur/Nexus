#include <nexus/Socket.hpp>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <iostream>

namespace Nexus {

    Socket::Socket(std::any handle) : handle(std::move(handle)) {}

    Socket::Socket(Socket && socket) noexcept : handle(std::move(socket.handle)) {
        socket.handle = INVALID_SOCKET;
    }

    Socket::~Socket() {
        if (std::any_cast<SOCKET>(handle) != INVALID_SOCKET) {
            closesocket(std::any_cast<SOCKET>(handle));
        }
    }

    std::size_t Socket::write(Buffer const & buffer, std::size_t count) {
        // send via send
        return 0;
    }

    std::size_t Socket::read(Buffer & buffer, std::size_t count) {
        // receive via recv
        return 0;
    }
}