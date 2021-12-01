#pragma once

#include <cstdint>
#include <string>

namespace Nexus {

    class ServerSocket;
    class Socket;

    void init();

    void shutdown();

    ServerSocket open(std::uint16_t port);

    Socket connect(std::string const & host, std::uint16_t port);
}