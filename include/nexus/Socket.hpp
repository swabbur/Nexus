#pragma once

#include <any>
#include <string>

namespace Nexus {

    class Buffer;

    class Socket {

        friend class Selector;

        std::any handle;

    public:

        static Socket connect(std::string const & host, std::uint16_t port);

        explicit Socket(std::any handle);

        Socket(Socket const & socket) = delete;

        Socket(Socket && socket) noexcept;

        ~Socket();

        void write(Buffer & buffer);

        void read(Buffer & buffer);
    };
}
