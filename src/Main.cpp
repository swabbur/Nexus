//#include <nexus/util/Thread.hpp>
//#include <nexus/Exception.hpp>
//#include <nexus/Selector.hpp>
//#include <nexus/ServerSocket.hpp>
//#include <nexus/Socket.hpp>
//#include <iostream>
//#include <stdexcept>

#include <cstdint>
#include <mutex>
#include <vector>
#include <iostream>

template<typename Value>
class Codec {

public:

    static void serialize(Value const & value, std::vector<std::byte>::iterator const & output_iterator) {
        auto const * bytes = reinterpret_cast<std::byte const *>(&value);
        std::copy_n(bytes, sizeof(Value), output_iterator);
    }

    static Value deserialize(std::vector<std::byte>::iterator const & input_iterator) {
        std::byte bytes[sizeof(Value)];
        std::copy_n(input_iterator, sizeof(Value), bytes);
        return *reinterpret_cast<Value *>(bytes);
    }

    Codec() = delete;
};

template<>
class Codec<std::string> {

public:

    static void serialize(std::string const & value, std::vector<std::byte>::iterator & output_iterator) {
        auto const * bytes = reinterpret_cast<std::byte const *>(value.data());
        std::copy_n(bytes, value.size() + 1, output_iterator);
    }

    static std::string deserialize(std::vector<std::byte>::iterator & input_iterator) {
        std::string value;
        auto current = static_cast<std::string::value_type>(*input_iterator++);
        while (current != 0) {
            value.push_back(current);
            current = static_cast<std::string::value_type>(*input_iterator++);
        }
        return value;
    }

    Codec() = delete;
};

class Buffer {

    // TODO: Replace with queue?
    std::vector<std::byte> bytes;
    std::size_t position;

public:

    explicit Buffer(std::size_t size) : bytes(size, std::byte(0)), position(0) {}

    template<typename Value>
    Value read() {
        auto iterator = bytes.begin() + position;
        return Codec<Value>::deserialize(iterator); // NOLINT(cppcoreguidelines-narrowing-conversions)
    }

    template<typename Value>
    void write(Value const & value) {
        auto iterator = bytes.begin() + position;
        Codec<Value>::serialize(value, iterator); // NOLINT(cppcoreguidelines-narrowing-conversions)
    }

    void rewind() {
        position = 0;
    }
};

#include <array>

int main() {

    Buffer buffer(1024);
    buffer.write<std::string>("Hello, Nexus!");
    buffer.rewind();
    std::cout << buffer.read<std::string>() << std::endl;

    // TODO: Improve Codec interface s.t. it can indicate how many bytes were generated or used.

    return 0;

//    try {
//        // Setup server
//        Nexus::ServerSocket server_socket = Nexus::ServerSocket::open(2302);
//        std::vector<Nexus::Socket> client_sockets;
//
//        // Create selector
//        Nexus::Selector selector;
//
//        selector.attach(server_socket, [&server_socket, &client_sockets, &selector]() {
//            client_sockets.push_back(server_socket.accept());
//            Nexus::Socket & client = client_sockets.back();
//            selector.attach(client, [&client]() {
//                std::string message = client.read();
//                std::cout << message << std::endl;
//            });
//        });
//
//        std::atomic_bool running = true;
//        Nexus::Thread server_thread([&running, &selector](){
//            while (running) {
//                selector.poll();
//            }
//        });
//
//        // Connect client
//        Nexus::Socket socket = Nexus::Socket::connect("127.0.0.1", 2302);
//
//        socket.write("Hello, Nexus!");
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        socket.write("Hello!");
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        running = false;
//
//        return 0;
//
//    } catch (Nexus::Exception const & exception) {
//        std::cerr << exception.what() << std::endl;
//        return 1;
//    }

}
