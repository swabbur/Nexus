#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>
#include <utility>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

namespace Nexus {

    ServerSocket::ServerSocket(std::any handle) : handle(std::move(handle)) {

    }

    ServerSocket::ServerSocket(ServerSocket && server_socket) noexcept : handle(std::move(server_socket.handle)) {
        server_socket.handle = INVALID_SOCKET;
    }

    ServerSocket::~ServerSocket() {
        if (std::any_cast<SOCKET>(handle) != INVALID_SOCKET) {
            closesocket(std::any_cast<SOCKET>(handle));
        }
    }

    Socket ServerSocket::accept() {
        return Socket(INVALID_SOCKET);
    }
}
