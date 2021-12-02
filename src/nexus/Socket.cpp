#include "WinSock.hpp"

#include <nexus/Socket.hpp>
#include <array>
#include <iostream>

namespace Nexus {

    static thread_local std::array<char, 1024> READ_BUFFER;

    Socket Socket::connect(std::string const & host, std::uint16_t port) {

        WinSock::init();

        SOCKET handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (handle == INVALID_SOCKET) {
            int error = WSAGetLastError();
            std::cerr << "Could not create socket (" << error << ')' << std::endl;
            std::exit(1);
        }

        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(host.c_str()); // inet_pton()
        address.sin_port = htons(port);


        int connected = ::connect(handle, (SOCKADDR *) &address, sizeof(address));
        if (connected == SOCKET_ERROR) {
            int error = WSAGetLastError();
            std::cerr << "Could not connect socket (" << error << ')' << std::endl;
            std::exit(1);
        }

        return Socket(handle);
    }

    Socket::Socket(std::any handle) : handle(std::move(handle)) {
        if (handle.type() != typeid(SOCKET)) {
            std::cerr << "Socket handle must be of type SOCKET" << std::endl;
            std::exit(1);
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
                std::cerr << "Could not close socket (" << error << ')' << std::endl;
                std::exit(1);
            }

            WinSock::shutdown();
        }
    }

    void Socket::write(std::string const & message) {
        int bytes_sent = ::send(std::any_cast<SOCKET>(handle), message.c_str(), message.size() + 1, 0);
        if (bytes_sent == SOCKET_ERROR) {
            int error = WSAGetLastError();
            std::cerr << "Could not send message (" << error << ')' << std::endl;
            std::exit(1);
        }
    }

    std::string Socket::read() {

        int bytes_received = ::recv(std::any_cast<SOCKET>(handle), READ_BUFFER.data(), READ_BUFFER.size(), 0);
        if (bytes_received == SOCKET_ERROR) {
            int error = WSAGetLastError();
            std::cerr << "Could not receive message (" << error << ')' << std::endl;
            std::exit(1);
        }

        return { READ_BUFFER.data() };
    }
}
