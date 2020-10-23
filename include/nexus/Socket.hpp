#pragma once

#include <string>

class Socket {

    int descriptor;

public:

    static Socket connect(std::string const & host, int port);

    explicit Socket(int descriptor);

    Socket(Socket const &) = delete;

    Socket(Socket && client) noexcept;

    ~Socket();

    [[nodiscard]] int get_descriptor() const;

    [[maybe_unused]] std::size_t read(char * bytes, std::size_t length);

    std::size_t write(char * bytes, std::size_t length);
};
