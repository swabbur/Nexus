#include "WinSock.hpp"

#include <nexus/Buffer.hpp>
#include <nexus/Exception.hpp>
#include <nexus/Socket.hpp>
#include <array>

namespace Nexus {

    static thread_local std::array<char, 1024> READ_BUFFER;

    Socket Socket::connect(std::string const & host, std::uint16_t port) {

        WinSock::init();

        SOCKET handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (handle == INVALID_SOCKET) {
            int error = WSAGetLastError();
            throw Exception("Could not create socket: ", error);
        }

        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(host.c_str()); // inet_pton()
        address.sin_port = htons(port);

        int connected = ::connect(handle, (SOCKADDR *) &address, sizeof(address));
        if (connected == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not connect socket: ", error);
        }

        return Socket(handle);
    }

    Socket::Socket(std::any handle) : handle(std::move(handle)) {
        if (handle.type() != typeid(SOCKET)) {
            throw Exception("Socket handle must be of type SOCKET");
        }
    }

    Socket::Socket(Socket && socket) noexcept : handle(std::move(socket.handle)) {
        socket.handle = INVALID_SOCKET;
    }

    Socket::~Socket() {
        if (std::any_cast<SOCKET>(handle) != INVALID_SOCKET) {

            int closed = ::closesocket(std::any_cast<SOCKET>(handle));
            if (closed == SOCKET_ERROR) {
                int error = WSAGetLastError();
                throw Exception("Could not close socket: ", error);
            }

            WinSock::shutdown();
        }
    }

    void Socket::write(Buffer & buffer) {
        std::vector<std::byte> bytes = buffer.read_all();
        int bytes_sent = ::send(std::any_cast<SOCKET>(handle), reinterpret_cast<const char *>(bytes.data()), bytes.size() + 1, 0);
        if (bytes_sent == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not send message: ", error);
        }
    }

    void Socket::read(Buffer & buffer) {

        int bytes_received = ::recv(std::any_cast<SOCKET>(handle), READ_BUFFER.data(), READ_BUFFER.size(), 0);
        if (bytes_received == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not receive message: ", error);
        }

        if (bytes_received == 0) {
            buffer.write<std::byte>(std::byte(0));
            return;
        }

        for (std::size_t index = 0; index < bytes_received; index++) {
            buffer.write(READ_BUFFER[index]);
        }
    }
}
