//#include <nexus/Buffer.hpp>
//#include <nexus/Codec.hpp>
//#include <iostream>
//
//int main() {
//    try {
//        Nexus::Buffer buffer(1024);
//        buffer.push<uint16_t>(2302);;
//        buffer.push<float>(2.0f);
//        buffer.push<std::string>("Hello, World!");
//        buffer.push<std::array<int, 4>>({ 1, 2, 3, 4 });
//        std::cout << buffer.pop<uint16_t>() << std::endl;
//        std::cout << buffer.pop<float>() << std::endl;
//        std::cout << buffer.pop<std::string>() << std::endl;
//        for (int value : buffer.pop<std::array<int, 4>>()) {
//            std::cout << value << ' ';
//        }
//        std::cout << std::endl;
//        return 0;
//    } catch (std::exception const & exception) {
//        std::cerr << exception.what() << std::endl;
//        return 1;
//    }
//}

//#include <cstddef>
//#include <cstdint>
//#include <iostream>
//
//template<std::size_t DIMENSIONS>
//class Position;
//
//template<>
//class Position<3> {
//
//    typedef float Position::* const ComponentPointer;
//    static ComponentPointer const component_pointers[3];
//
//public:
//
//    float x, y, z;
//
//    Position(float x, float y, float z) : x(x), y(y), z(z) {}
//
//    explicit Position(float scalar) : x(scalar), y(scalar), z(scalar) {}
//
//    Position() : x(0.0f), y(0.0f), z(0.0f) {}
//
//    float const & operator [] (std::size_t index) const {
//        return this->*component_pointers[index];
//    }
//
//    float & operator [] (std::size_t index) {
//        return this->*component_pointers[index];
//    }
//};
//
//Position<3>::ComponentPointer const Position<3>::component_pointers[3] = {&Position<3>::x, &Position<3>::y, &Position<3>::z };
//
//int main() {
//    Position<3> position(1.0f, 2.0f, 3.0f);
//    std::cout << position.x << ' ' << position[0] << std::endl;
//    std::cout << position.y << ' ' << position[1] << std::endl;
//    std::cout << position.z << ' ' << position[2] << std::endl;
//    return 0;
//}

#include <nexus/util/Thread.hpp>
#include <nexus/Buffer.hpp>
#include <nexus/Codec.hpp>
#include <nexus/Exception.hpp>
#include <nexus/Selector.hpp>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>
#include <iostream>
#include <stdexcept>

// TODO: Increase buffer size s.t. no wrap-around is necessary for reading and writing data. This would speed up buffer
//  use as no copies are required until the move operation of reinterpreted pointers.

int main() {

    Nexus::Thread server_thread([](){
        try {

            Nexus::Buffer buffer(1024);

            Nexus::ServerSocket server_socket = Nexus::ServerSocket::open(2302);
            Nexus::Socket client_socket = server_socket.accept();
            client_socket.read(buffer);

            std::cout << buffer.pop<std::string>() << std::endl;
            std::cout << buffer.pop<std::string>() << std::endl;
            std::cout << buffer.pop<int>() << std::endl;

        } catch (Nexus::Exception const & exception) {
            std::cerr << exception.what() << std::endl;
        }
    });

    Nexus::Thread client_thread([](){
        try {

            Nexus::Buffer buffer(1024);
            buffer.push<std::string>("Hello, Nexus!");
            buffer.push<std::string>("Hello!");
            buffer.push<int>(2302);

            Nexus::Socket socket = Nexus::Socket::connect("127.0.0.1", 2302);
            socket.write(buffer);

        } catch (Nexus::Exception const & exception) {
            std::cerr << exception.what() << std::endl;
        }
    });

    return 0;
}
