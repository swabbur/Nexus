#pragma once

#include <functional>
#include <map>
#include <mutex>

namespace Nexus {

    class ServerSocket;
    class Socket;

    class Selector {

        std::map<ServerSocket const *, std::function<void()>> accept_callbacks;
        std::map<Socket const *, std::function<void()>> read_callbacks;
        std::mutex mutex;

    public:

        Selector();

        Selector(Selector const & selector) = delete;

        Selector(Selector && selector) noexcept = delete;

        ~Selector();

        void attach(ServerSocket const & server_socket, std::function<void()> accept_callback);

        void attach(Socket const & socket, std::function<void()> read_callback);

        void detach(ServerSocket const & server_socket);

        void detach(Socket const & socket);

        void poll();
    };
}
