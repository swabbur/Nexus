#include <nexus/Buffer.hpp>
#include <nexus/Codec.hpp>
#include <nexus/Selector.hpp>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>
#include <iostream>
#include <stdexcept>
#include <thread>

// TODO: Allow bit_casting data stored in the buffer directly to a type, eliminating the intermediate data copy.
// TODO: Provide sink/flush functions to directly copy data to/from the buffer without additional copy.

int main() {

    std::jthread server_thread([](){
        try {

            Nexus::ServerSocket server_socket = Nexus::ServerSocket::open(2302);
            Nexus::Socket client_socket = server_socket.accept();

            Nexus::Buffer buffer(1024);
            client_socket.receive(buffer);

            std::cout << buffer.read<std::string>() << std::endl;
            std::cout << buffer.read<std::string>() << std::endl;
            std::cout << buffer.read<int>() << std::endl;

        } catch (std::exception const & exception) {
            std::cerr << exception.what() << std::endl;
        }
    });

    std::jthread client_thread([](){
        try {

            Nexus::Buffer buffer(1024);
            buffer.write<std::string>("Hello, Nexus!");
            buffer.write<std::string>("Hello!");
            buffer.write<int>(2302);

            Nexus::Socket socket = Nexus::Socket::connect("127.0.0.1", 2302);
            socket.send(buffer);

        } catch (std::exception const & exception) {
            std::cerr << exception.what() << std::endl;
        }
    });

    return 0;
}
