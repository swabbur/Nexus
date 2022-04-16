#pragma once

#include "Handle.hpp"

#include <string>

// TODO: Consider separating opening and connecting a socket (s.t. a socket can be reused and reconnected).
// TODO: Track the current socket state (OPEN, CONNECTED, CLOSED).

namespace Nexus {

    class Socket {

        Handle handle;

    public:

        [[nodiscard]] static Socket connect(std::string const & host, std::uint16_t port);

        [[nodiscard]] explicit Socket(Handle handle) noexcept;

        Socket(Socket const & socket) = delete;

        Socket(Socket && socket) noexcept;

        ~Socket() noexcept(false);

        [[nodiscard]] Handle get_handle() const noexcept;

        std::size_t send(void * data, std::size_t size);

        std::size_t receive(void * data, std::size_t size);
    };
}
