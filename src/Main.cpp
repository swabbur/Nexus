#include <nexus/util/Thread.hpp>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>
#include <iostream>

// TODO: Create non-blocking IO.

// TODO: Implement selector.

// TODO: Implement proper error handling.

int main() {

    Nexus::Thread server_thread([](){
        Nexus::ServerSocket server_socket = Nexus::ServerSocket::open(2302);
        Nexus::Socket client_socket = server_socket.accept();
        std::cout << client_socket.read() << std::endl;
    });

    Nexus::Socket socket = Nexus::Socket::connect("127.0.0.1", 2302);
    socket.write("Hello");

    return 0;
}
