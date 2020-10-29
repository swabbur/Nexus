#pragma once

#include <functional>
#include <poll.h>
#include <vector>

class ServerSocket;
class Socket;

class Selector {

    std::vector<struct pollfd> pollfds;
    std::vector<std::function<void()>> callbacks;

public:

    Selector();

    Selector(Selector const &) = delete;

    Selector(Selector && selector) noexcept;

    ~Selector();

    void attach(ServerSocket const & server_socket, std::function<void()> const & callback);

    void attach(Socket const & socket, std::function<void()> const & callback);

    void detach(ServerSocket const & server_socket);

    void detach(Socket const & socket);

    void poll();
};