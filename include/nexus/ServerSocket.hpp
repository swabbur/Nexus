#pragma once

class Socket;

class ServerSocket {

public:

    static ServerSocket open(int port);

    ServerSocket();

    ServerSocket(ServerSocket const &) = delete;

    ServerSocket(ServerSocket && server) noexcept;

    ~ServerSocket();

    [[nodiscard]] bool is_open() const;

    [[nodiscard]] Socket accept();
};
