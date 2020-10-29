#include <nexus/Selector.hpp>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>

Selector::Selector() : pollfds(), callbacks() {}

Selector::Selector(Selector && selector) noexcept :
    pollfds(std::move(selector.pollfds)),
    callbacks(std::move(selector.callbacks))
{}

Selector::~Selector() = default;

void Selector::attach(ServerSocket const & server_socket, std::function<void()> const & callback) {
    pollfds.push_back({ server_socket.get_descriptor(), POLLIN, 0 });
    callbacks.emplace_back(callback);
}

void Selector::attach(Socket const & socket, std::function<void()> const & callback) {
    pollfds.push_back({ socket.get_descriptor(), POLLIN, 0 });
    callbacks.push_back(callback);
}

void Selector::detach(ServerSocket const & server_socket) {
    for (std::size_t index = 0; index < pollfds.size(); index++) {
        if (pollfds[index].fd == server_socket.get_descriptor()) {
            pollfds[index].fd = -1;
            callbacks[index] = nullptr;
        }
    }
}

void Selector::detach(Socket const & socket) {
    for (std::size_t index = 0; index < pollfds.size(); index++) {
        if (pollfds[index].fd == socket.get_descriptor()) {
            pollfds[index].fd = -1;
            callbacks[index] = nullptr;
        }
    }
}

void Selector::poll() {

    for (std::size_t index = 0, offset = 0; index < pollfds.size(); index++) {
        if (pollfds[index].fd < 0) {
            offset += 1;
        } else {
            pollfds[index - offset] = pollfds[index];
            callbacks[index - offset] = callbacks[index];
        }
    }

    int ready_count = ::poll(pollfds.data(), pollfds.size(), -1);
    if (ready_count < 1) {
        throw std::runtime_error("Could not poll sockets");
    }

    for (std::size_t index = 0; index < pollfds.size(); index++) {
        if (pollfds[index].revents & POLLIN && callbacks[index]) {
            callbacks[index]();
        }
    }
}
