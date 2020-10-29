#include <nexus/Selector.hpp>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>
#include <atomic>
#include <iostream>
#include <thread>

int main() {

    std::atomic_bool server_ready = false;
    std::thread server_thread([&server_ready](){

        ServerSocket server_socket = ServerSocket::open(2302);
        server_ready = true;

        std::vector<Socket> echo_sockets;

        Selector selector;
        selector.attach(server_socket, [&server_socket, &echo_sockets, &selector]() {
            echo_sockets.push_back(server_socket.accept());
            Socket & echo_socket = echo_sockets.back();
            selector.attach(echo_sockets.back(), [&echo_socket]() {
                std::array<char, 256> buffer = {};
                int bytes_read = echo_socket.read(buffer.data(), buffer.size());
                echo_socket.write(buffer.data(), bytes_read);
            });
        });

        while (server_socket.is_open()) {
            selector.poll();
        }
    });

    while (!server_ready);

    Socket client_socket = Socket::connect("localhost", 2302);
    std::string message;
    std::array<char, 256> buffer = {};
    while (client_socket.is_open()) {
        std::cout << "Please, enter a message: ";
        std::getline(std::cin, message);
        if (message == "exit") {
            break;
        }
        client_socket.write(message.c_str(), message.length() + 1);
        client_socket.read(buffer.data(), buffer.size());
        std::cout << "Received message: " << buffer.data() << std::endl;
    }

    // Wait for server to close
    if (server_thread.joinable()) {
        server_thread.join();
    }
}