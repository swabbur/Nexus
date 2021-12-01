#pragma once

#include <string>

class Socket {

public:

    static Socket connect(std::string const & host, int port);

    explicit Socket();

    Socket(Socket const &) = delete;

    Socket(Socket && client) noexcept;

    ~Socket();

    [[nodiscard]] bool is_open() const;

    [[maybe_unused]] std::size_t read(char * bytes, std::size_t length);

    std::size_t write(char const * bytes, std::size_t length);
};
