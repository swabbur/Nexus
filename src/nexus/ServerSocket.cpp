#include "WinSock.hpp"

#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>
#include <iostream>
#include <utility>

namespace Nexus {

    ServerSocket ServerSocket::open(std::uint16_t port) {

        WinSock::init();

        SOCKET handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (handle == INVALID_SOCKET) {
            int error = WSAGetLastError();
            std::cerr << "Could not create socket_handle (" << error << ')' << std::endl;
            std::exit(1);
        }

        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr("127.0.0.1"); // inet_pton()
        address.sin_port = htons(port);

        int bound = ::bind(handle, (SOCKADDR *)&address, sizeof(address));
        if (bound == SOCKET_ERROR) {
            int error = WSAGetLastError();
            std::cerr << "Could not bind socket (" << error << ')' << std::endl;
            std::exit(1);
        }

        int listening = ::listen(handle, SOMAXCONN);
        if (listening == SOCKET_ERROR) {
            int error = WSAGetLastError();
            std::cerr << "Could not listen on socket (" << error << ')' << std::endl;
            std::exit(1);
        }

        return ServerSocket(handle);
    }

    ServerSocket::ServerSocket(std::any handle) : handle(std::move(handle)) {
        if (handle.type() != typeid(SOCKET)) {
            std::cerr << "Socket handle must be of type SOCKET" << std::endl;
            std::exit(1);
        }
    }

    ServerSocket::ServerSocket(ServerSocket && server_socket) noexcept : handle(std::move(server_socket.handle)) {
        server_socket.handle = INVALID_SOCKET;
    }

    ServerSocket::~ServerSocket() {
        if (std::any_cast<SOCKET>(handle) != INVALID_SOCKET) {

            int closed = ::closesocket(std::any_cast<SOCKET>(handle));
            if (closed == SOCKET_ERROR) {
                int error = WSAGetLastError();
                std::cerr << "Could not close socket (" << error << ')' << std::endl;
                std::exit(1);
            }

            WinSock::shutdown();
        }
    }

    Socket ServerSocket::accept() {

        WinSock::init();

        SOCKET client_handle = ::accept(std::any_cast<SOCKET>(handle), nullptr, nullptr);
        if (client_handle == INVALID_SOCKET) {
            int error = WSAGetLastError();
            std::cerr << "Could not accept socket (" << error << ')' << std::endl;
            std::exit(1);
        }

        return Nexus::Socket(client_handle);
    }
}
