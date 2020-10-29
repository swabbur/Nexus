#include <netinet/in.h>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>

ServerSocket ServerSocket::open(int port) {

    int socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) {
        throw std::runtime_error("Could not open server socket");
    }

    struct sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        throw std::runtime_error("Could not bind server socket");
    }

    if (listen(socket, 5) < 0) {
        throw std::runtime_error("Could not listen via server socket");
    }

    return ServerSocket(socket);
}

ServerSocket::ServerSocket(int descriptor) : descriptor(descriptor) {}

ServerSocket::ServerSocket(ServerSocket && server) noexcept : descriptor(server.descriptor) {
    server.descriptor = -1;
}

ServerSocket::~ServerSocket() {
    ::close(descriptor);
    descriptor = -1;
}

int ServerSocket::get_descriptor() const {
    return descriptor;
}

bool ServerSocket::is_open() const {
    return descriptor != -1;
}

Socket ServerSocket::accept() { // NOLINT(readability-make-member-function-const)

    struct sockaddr_in address = {};
    socklen_t length = sizeof(address);
    int socket = ::accept(descriptor, (struct sockaddr *) &address, &length);
    if (socket < 0) {
        throw std::runtime_error("Could not accept client socket");
    }

    return Socket(socket);
}
