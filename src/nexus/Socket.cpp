#include <netinet/in.h>
#include <nexus/Socket.hpp>
#include <sys/socket.h>
#include <algorithm>
#include <netdb.h>
#include <stdexcept>
#include <unistd.h>

Socket Socket::connect(std::string const & host, int port) {

    int file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (file_descriptor < 0) {
        throw std::runtime_error("Could not open client socket");
    }

    struct hostent * server = gethostbyname(host.c_str());
    if (server == nullptr) {
        throw std::runtime_error("Host does not exist");
    }

    struct sockaddr_in address = {};
    address.sin_family = AF_INET;
    std::copy_n((char *) server->h_addr, server->h_length, (char *) &address.sin_addr.s_addr);
    address.sin_port = htons(port);

    if (::connect(file_descriptor, (struct sockaddr *) &address, sizeof(address)) < 0) {
        throw std::runtime_error("Cannot connect to host");
    }

    return Socket(file_descriptor);
}

Socket::Socket(int descriptor) : descriptor(descriptor) {}

Socket::Socket(Socket && client) noexcept : descriptor(client.descriptor) {
    client.descriptor = -1;
}

Socket::~Socket() {
    ::close(descriptor);
    descriptor = -1;
}

int Socket::get_descriptor() const {
    return descriptor;
}

bool Socket::is_open() const {
    return descriptor != -1;
}

std::size_t Socket::read(char * bytes, std::size_t length) { // NOLINT(readability-make-member-function-const)
    std::size_t bytes_read = ::read(descriptor, bytes, length);
    if (bytes_read < 0) {
        throw std::runtime_error("Could not read from socket");
    }
    return bytes_read;
}

std::size_t Socket::write(char const * bytes, std::size_t length) { // NOLINT(readability-make-member-function-const)
    std::size_t bytes_written = ::write(descriptor, bytes, length);
    if (bytes_written < 0) {
        throw std::runtime_error("Could not write to socket");
    }
    return bytes_written;
}
