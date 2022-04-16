#pragma once

#include "../Handle.hpp"

namespace Nexus {

    class Socket;

    class ServerSocket {

        Handle handle;

    public:

        [[nodiscard]] static ServerSocket open(std::uint16_t port);

        [[nodiscard]] explicit ServerSocket(Handle handle) noexcept;

        ServerSocket(ServerSocket const & server_socket) = delete;

        ServerSocket(ServerSocket && server_socket) noexcept;

        ~ServerSocket() noexcept(false);

        [[nodiscard]] Handle get_handle() const noexcept;

        Socket accept();
    };
}
