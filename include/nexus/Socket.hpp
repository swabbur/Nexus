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

        ~Socket() noexcept(false);

        std::size_t send(void * data, std::size_t size);

        std::size_t receive(void * data, std::size_t size);

        void send(Buffer & buffer);

        void receive(Buffer & buffer);
    };
}
