#pragma once

#include <any>

namespace Nexus {

    class Socket;

    class ServerSocket {

        friend class Selector;

        std::any handle;

    public:

        static ServerSocket open(std::uint16_t port);

        explicit ServerSocket(std::any  handle);

        ServerSocket(ServerSocket const & server_socket) = delete;

        ServerSocket(ServerSocket && server_socket) noexcept;

        ~ServerSocket();

        Socket accept();
    };
}
