#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>

ServerSocket ServerSocket::open(int port) {
    return ServerSocket();
}

ServerSocket::ServerSocket() = default;

ServerSocket::ServerSocket(ServerSocket && server) noexcept = default;

ServerSocket::~ServerSocket() = default;

bool ServerSocket::is_open() const {
    return false;
}

Socket ServerSocket::accept() {
    return Socket();
}
