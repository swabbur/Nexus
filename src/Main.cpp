#include <nexus/Buffer.hpp>
#include <nexus/Codec.hpp>
#include <nexus/Exception.hpp>
#include <nexus/Selector.hpp>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>
#include <iostream>
#include <stdexcept>
#include <thread>

// TODO: Increase buffer size s.t. no wrap-around is necessary for reading and writing data. This would speed up buffer
//  use as no copies are required until the move operation of reinterpreted pointers.

// TODO: Provide sink/flush functions to directly copy data to/from the buffer without additional copy.

// TODO: Add optional returning try_pop/try_push s.t. they can be used safely from the selector callbacks.



// !!!!!!!!!!!!!!!!!!!!!!
// TODO: Use std::bit_cast and std::jthread!
// !!!!!!!!!!!!!!!!!!!!!!



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
