#pragma once

#include <any>

namespace Nexus {

    class Buffer;

    class Socket {

        std::any handle;

    public:

        explicit Socket(std::any handle);

        Socket(Socket const & socket) = delete;

        Socket(Socket && socket) noexcept;

        ~Socket();

        [[maybe_unused]] std::size_t write(Buffer const & buffer, std::size_t count);

        [[maybe_unused]] std::size_t read(Buffer & buffer, std::size_t count);
    };
}
