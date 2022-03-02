#include "WinSock.hpp"

#include <nexus/Buffer.hpp>
#include <nexus/Exception.hpp>
#include <nexus/Socket.hpp>
#include <array>
#include <stdexcept>

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

    std::size_t Socket::send(void * data, std::size_t size) {
        int bytes_sent = ::send(std::any_cast<SOCKET>(handle), static_cast<char const *>(data), static_cast<int>(size), 0);
        if (bytes_sent == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not send message: ", error);
        } else {
            return bytes_sent;
        }
    }

    std::size_t Socket::receive(void * data, std::size_t size) {
        int bytes_received = ::recv(std::any_cast<SOCKET>(handle), static_cast<char *>(data), static_cast<int>(size), 0);
        if (bytes_received == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not receive message: ", error);
        } else if (bytes_received == 0) {
            throw std::runtime_error("Socket closed");
        } else {
            return bytes_received;
        }
    }

    void Socket::send(Buffer & buffer) {
        std::size_t bytes_written = send(&buffer.bytes[buffer.head], std::min<>(buffer.count, buffer.capacity - buffer.head));
        if (bytes_written == buffer.capacity - buffer.head) {
            bytes_written += send(&buffer.bytes, buffer.count - (buffer.capacity - buffer.head));
        }
        buffer.head = (buffer.head + bytes_written) % buffer.capacity;
        buffer.count -= bytes_written;
    }

    void Socket::receive(Buffer & buffer) {
        std::size_t bytes_read = receive(&buffer.bytes[buffer.tail], buffer.capacity - buffer.tail);
        if (bytes_read == buffer.capacity - buffer.tail) {
            bytes_read += receive(&buffer.bytes, buffer.head - 1);
        }
        buffer.tail = (buffer.tail + bytes_read) % buffer.capacity;
        buffer.count += bytes_read;
    }
}
