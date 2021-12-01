#include <nexus/Socket.hpp>

Socket Socket::connect(std::string const & host, int port) {
    return Socket();
}

Socket::Socket() = default;

Socket::Socket(Socket && client) noexcept = default;

Socket::~Socket() = default;

bool Socket::is_open() const {
    return false;
}

std::size_t Socket::read(char * bytes, std::size_t length) {
    return 0;
}

std::size_t Socket::write(char const * bytes, std::size_t length) {
    return 0;
}
