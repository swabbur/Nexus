#pragma once

#include <functional>

class ServerSocket;
class Socket;

class Selector {

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