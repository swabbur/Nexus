#include <nexus/Selector.hpp>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>

Selector::Selector() = default;

Selector::Selector(Selector && selector) noexcept = default;

Selector::~Selector() = default;

void Selector::attach(ServerSocket const & server_socket, std::function<void()> const & callback) {

}

void Selector::attach(Socket const & socket, std::function<void()> const & callback) {

}

void Selector::detach(ServerSocket const & server_socket) {

}

void Selector::detach(Socket const & socket) {

}

void Selector::poll() {

}
