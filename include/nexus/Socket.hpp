#pragma once

#include <any>
#include <string>

namespace Nexus {

    class Buffer;

    class Socket {

        std::any handle;

    public:

        static Socket connect(std::string const & host, std::uint16_t port);

        explicit Socket(std::any handle);

        Socket(Socket const & socket) = delete;

        Socket(Socket && socket) noexcept;

        ~Socket();

        void write(std::string const & message);

        std::string read();
    };
}
