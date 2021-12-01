#pragma once

#include <any>

namespace Nexus {

    class Socket;

    class ServerSocket {

        std::any handle;

    public:

        explicit ServerSocket(std::any  handle);

        ServerSocket(ServerSocket const & server_socket) = delete;

        ServerSocket(ServerSocket && server_socket) noexcept;

        ~ServerSocket();

        Socket accept();
    };
}
