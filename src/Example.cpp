#include <nexus/tcp/ServerSocket.hpp>
#include <nexus/tcp/Socket.hpp>
#include <nexus/Selector.hpp>
#include <iostream>

// https://wiki.vg/Protocol

void server() {
    try {

        auto server_socket = Nexus::ServerSocket::open(2302);

        std::atomic_bool running = true;
        Nexus::Selector selector;
        selector.attach(server_socket, [&server_socket, &running]() {
            std::cout << "Callback activated" << std::endl;
            auto client_socket = server_socket.accept();
            std::cout << "Client accepted" << std::endl;
            running = false;
        });

        while (running) {
            selector.poll();
        }

    } catch (std::exception const & exception) {
        std::cerr << exception.what() << std::endl;
    }
}

void client() {
    try {
        auto socket = Nexus::Socket::connect("127.0.0.1", 2302);
    } catch (std::exception const & exception) {
        std::cerr << exception.what() << std::endl;
    }
}

int main() {

    // Server bound:
    // TODO: Figure out how to properly define non-blocking sockets and a selector.
    // TODO: See if I can use coroutines for constructing packets (data may arrive incomplete).
    // TODO: Transform packets into (context-independent) server events.

    // Client bound:
    // TODO: Server provides (context-independent) client events.
    // TODO: Client events are matched against connected clients to figure out who is interested in which event.
    // TODO: Packets are produced and enqueued for interested clients.
    // TODO: Once a certain 'importance' threshold is met, the packets are send to the client.

    // Notes:
    // TODO: Use a thread pool to perform all these actions in parallel.

    std::jthread server_thread(server);
    std::jthread client_thread(client);
    return 0;
}