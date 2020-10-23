#pragma once

class Socket;

class ServerSocket {

    int descriptor;

public:

    static ServerSocket open(int port);

    explicit ServerSocket(int descriptor);

    ServerSocket(ServerSocket const &) = delete;

    ServerSocket(ServerSocket && server) noexcept;

    ~ServerSocket();

    [[nodiscard]] int get_descriptor() const;

    [[nodiscard]] Socket accept();
};
