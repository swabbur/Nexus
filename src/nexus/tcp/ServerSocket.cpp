#include "WinSock.hpp"

#include "nexus/Exception.hpp"
#include "nexus/tcp/ServerSocket.hpp"
#include "nexus/tcp/Socket.hpp"

namespace Nexus {

    ServerSocket ServerSocket::open(std::uint16_t port) {

        WinSock::init();

        SOCKET handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (handle == INVALID_SOCKET) {
            int error = WSAGetLastError();
            throw Exception("Could not create socket handle", error);
        }

        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_port = htons(port);

        int translated = inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
        if (translated == 0) {
            throw Exception("Could not translate host address to an IPv4 address");
        }
        if (translated == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not translate host address: ", error);
        }

        int bound = ::bind(handle, (SOCKADDR *)&address, sizeof(address));
        if (bound == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not bind socket: ", error);
        }

        int listening = ::listen(handle, SOMAXCONN);
        if (listening == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not listen on socket: ", error);
        }

        u_long mode = 0;
        int changed = ioctlsocket(handle, FIONBIO, &mode);
        if (changed == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not configure server socket to be non-blocking: ", error);
        }

        return ServerSocket(handle);
    }

    ServerSocket::ServerSocket(Handle handle) noexcept : handle(handle) {}

    ServerSocket::ServerSocket(ServerSocket && server_socket) noexcept : handle(server_socket.handle) {
        server_socket.handle = INVALID_SOCKET;
    }

    ServerSocket::~ServerSocket() noexcept(false) {
        if (handle != INVALID_SOCKET) {

            int closed = ::closesocket(handle);
            if (closed == SOCKET_ERROR) {
                int error = WSAGetLastError();
                throw Exception("\"Could not close socket: ", error);
            }

            WinSock::shutdown();
        }
    }

    Handle ServerSocket::get_handle() const noexcept {
        return handle;
    }

    Socket ServerSocket::accept() { // NOLINT(readability-make-member-function-const)

        WinSock::init();

        SOCKET client_handle = ::accept(handle, nullptr, nullptr);
        if (client_handle == INVALID_SOCKET) {
            int error = WSAGetLastError();
            throw Exception("Could not accept socket: ", error);
        }

        return Nexus::Socket(client_handle);
    }
}
