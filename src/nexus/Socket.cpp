#include "WinSock.hpp"

#include "nexus/Exception.hpp"
#include "nexus/Socket.hpp"
#include <stdexcept>

namespace Nexus {

    Socket Socket::connect(std::string const & host, std::uint16_t port) {

        WinSock::init();

        SOCKET handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (handle == INVALID_SOCKET) {
            int error = WSAGetLastError();
            throw Exception("Could not create socket: ", error);
        }

        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_port = htons(port);

        int translated = inet_pton(AF_INET, host.c_str(), &address.sin_addr);
        if (translated == 0) {
            throw Exception("Could not translate host address to an IPv4 address");
        }
        if (translated == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not translate host address: ", error);
        }

        int connected = ::connect(handle, (SOCKADDR *) &address, sizeof(address));
        if (connected == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not connect socket: ", error);
        }

        u_long mode = 0;
        int configured = ioctlsocket(handle, FIONBIO, &mode);
        if (configured == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not configure socket to be non-blocking: ", error);
        }

        return Socket(handle);
    }

    Socket::Socket(Handle handle) noexcept : handle(handle) {}

    Socket::Socket(Socket && socket) noexcept : handle(socket.handle) {
        socket.handle = INVALID_SOCKET;
    }

    Socket::~Socket() noexcept(false) {
        if (handle != INVALID_SOCKET) {

            int closed = ::closesocket(handle);
            if (closed == SOCKET_ERROR) {
                int error = WSAGetLastError();
                throw Exception("Could not close socket: ", error);
            }

            WinSock::shutdown();
        }
    }

    Handle Socket::get_handle() const noexcept {
        return handle;
    }

    std::size_t Socket::send(void * data, std::size_t size) { // NOLINT(readability-make-member-function-const)
        int bytes_sent = ::send(handle, static_cast<char const *>(data), static_cast<int>(size), 0);
        if (bytes_sent == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not send message: ", error);
        } else {
            return bytes_sent;
        }
    }

    std::size_t Socket::receive(void * data, std::size_t size) { // NOLINT(readability-make-member-function-const)
        int bytes_received = ::recv(handle, static_cast<char *>(data), static_cast<int>(size), 0);
        if (bytes_received == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not receive message: ", error);
        } else if (bytes_received == 0) {
            throw Exception("Socket closed");
        } else {
            return bytes_received;
        }
    }
}
