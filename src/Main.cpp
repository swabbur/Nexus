#include <cstddef>
#include <iostream>
#include <array>
#include <vector>
#include <type_traits>

template<typename Value>
class Codec;

template<std::size_t CAPACITY>
class Buffer {

    std::array<std::byte, CAPACITY> bytes;
    std::size_t head;
    std::size_t tail;
    std::size_t count;

public:

    Buffer() : bytes(), head(0), tail(0), count(0) {}

    void read(void * data, std::size_t size) {

        if (size > count) {
            throw std::runtime_error("Insufficient readable memory available");
        }

        if (size <= CAPACITY - head) {
            auto iterator = std::begin(bytes);
            std::advance(iterator, head);
            std::copy_n(iterator, size, static_cast<std::byte *>(data));
        } else {

            auto input_iterator = std::begin(bytes);
            std::advance(input_iterator, head);
            std::copy_n(input_iterator, CAPACITY - head, static_cast<std::byte *>(data));

            auto output_iterator = static_cast<std::byte *>(data);
            std::advance(output_iterator, CAPACITY - head);
            std::copy_n(std::begin(bytes), size - (CAPACITY - tail), output_iterator);
        }

        head = (head + size) % CAPACITY;
        count -= size;
    }

    void write(void const * data, std::size_t size) {

        if (size > CAPACITY - count) {
            throw std::runtime_error("Insufficient writable memory available");
        }

        if (size <= CAPACITY - tail) {
            auto iterator = std::begin(bytes);
            std::advance(iterator, tail);
            std::copy_n(static_cast<std::byte const *>(data), size, iterator);
        } else {

            auto output_iterator = std::begin(bytes);
            std::advance(output_iterator, tail);
            std::copy_n(static_cast<std::byte const *>(data), CAPACITY - tail, output_iterator);

            auto input_iterator = static_cast<std::byte const *>(data);
            std::advance(input_iterator, CAPACITY - tail);
            std::copy_n(input_iterator, size - (CAPACITY - tail), std::begin(bytes));
        }

        tail = (tail + size) % CAPACITY;
        count += size;
    }

    template<typename Value>
    Value pop() {
        return Codec<Value>::deserialize(*this);
    }

    template<typename Value>
    void push(Value const & value) {
        Codec<Value>::serialize(value, *this);
    }
};

template<typename Value>
class Codec {

    static_assert(std::is_move_constructible_v<Value> && std::is_trivially_copyable_v<Value>,
            "The default codec is only guaranteed to work on types that are both move-constructible and trivially "
            "copyable");

public:

    template<std::size_t CAPACITY>
    static void serialize(Value const & value, Buffer<CAPACITY> & buffer) {
        buffer.write(&value, sizeof(Value));
    }

    template<std::size_t CAPACITY>
    static Value deserialize(Buffer<CAPACITY> & buffer) {
        std::array<std::byte, sizeof(Value)> bytes = {};
        buffer.read(bytes.data(), bytes.size());
        return std::move(*reinterpret_cast<Value *>(bytes.data()));
    }

    Codec() = delete;
};

template<typename Value, std::size_t SIZE>
class Codec<std::array<Value, SIZE>> {

    static_assert(std::is_default_constructible_v<Value> && std::is_move_constructible_v<Value>,
            "Array elements must be default-constructible and move-constructible to be deserialized");

public:

    template<std::size_t CAPACITY>
    static void serialize(std::array<Value, SIZE> const & values, Buffer<CAPACITY> & buffer) {
        for (Value const & value : values) {
            Codec<Value>::serialize(value, buffer);
        }
    }

    template<std::size_t CAPACITY>
    static std::array<Value, SIZE> deserialize(Buffer<CAPACITY> & buffer) {
        std::array<Value, SIZE> values;
        for (std::size_t index = 0; index < SIZE; index++) {
            values[index] = Codec<Value>::deserialize(buffer);
        }
        return values;
    }

    Codec() = delete;
};

template<typename Value>
class Codec<std::vector<Value>> {

    static_assert(std::is_move_constructible_v<Value>, "Vector elements must be move-constructible to be deserialized");

public:

    template<std::size_t CAPACITY>
    static void serialize(std::vector<Value> const & values, Buffer<CAPACITY> & buffer) {
        Codec<std::size_t>::serialize(values.size(), buffer);
        for (Value const & value : values) {
            Codec<Value>::serialize(value, buffer);
        }
    }

    template<std::size_t CAPACITY>
    static std::vector<Value> deserialize(Buffer<CAPACITY> & buffer) {
        std::vector<Value> values;
        std::size_t size = Codec<std::size_t>::deserialize(buffer);
        values.reserve(size);
        for (std::size_t index = 0; index < size; index++) {
            Value value = Codec<Value>::deserialize(buffer);
            values.push_back(std::move(value));
        }
        return values;
    }

    Codec() = delete;
};

template<>
class Codec<std::string> {

public:

    template<std::size_t CAPACITY>
    static void serialize(std::string const & value, Buffer<CAPACITY> & buffer) {
        buffer.write(value.c_str(), value.size() + 1);
    }

    template<std::size_t CAPACITY>
    static std::string deserialize(Buffer<CAPACITY> & buffer) {
        std::string value;
        char character;
        while ((character = Codec<char>::deserialize(buffer)) != '\0') {
            value += character;
        }
        return value;
    }

    Codec() = delete;
};

int main() {
    try {
        Buffer<1024> buffer;
        buffer.push<uint16_t>(2302);;
        buffer.push<float>(2.0f);
        buffer.push<std::string>("Hello, World!");
        buffer.push<std::array<int, 4>>({ 1, 2, 3, 4 });
        std::cout << buffer.pop<uint16_t>() << std::endl;
        std::cout << buffer.pop<float>() << std::endl;
        std::cout << buffer.pop<std::string>() << std::endl;
        for (int value : buffer.pop<std::array<int, 4>>()) {
            std::cout << value << ' ';
        }
        std::cout << std::endl;
        return 0;
    } catch (std::exception const & exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }
}

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

//#include <nexus/util/Thread.hpp>
//#include <nexus/Buffer.hpp>
//#include <nexus/Exception.hpp>
//#include <nexus/Selector.hpp>
//#include <nexus/ServerSocket.hpp>
//#include <nexus/Socket.hpp>
//#include <iostream>
//#include <stdexcept>

//#include <cstdint>
//#include <cstddef>
//#include <vector>
//#include <iostream>
//
//template<typename Value>
//class Codec;
//
//class Buffer {
//
//    template<typename>
//    friend class Codec;
//
//    std::vector<std::byte> bytes;
//    std::size_t head;
//    std::size_t tail;
//
//public:
//
//    explicit Buffer(std::size_t capacity);
//
//    template<typename Value>
//    void write(Value const & value) {
//        Codec<Value>::serialize(value, *this);
//    }
//
//    template<typename Value>
//    Value read() {
//        return Codec<Value>::deserialize(*this);
//    }
//
//    void clear();
//};
//
//Buffer::Buffer(std::size_t capacity) : bytes(capacity, std::byte(0)), head(0), tail(0) {}
//
//void Buffer::clear() {
//    std::fill(bytes.begin(), bytes.end(), std::byte(0));
//    head = 0;
//    tail = 0;
//}
//
//template<>
//class Codec<std::byte> {
//
//public:
//
//    static void serialize(std::byte const & value, Buffer & buffer) {
//        buffer.bytes[buffer.tail++] = value;
//    }
//
//    static std::byte deserialize(Buffer & buffer) {
//        return buffer.bytes[buffer.head++];
//    }
//
//    Codec() = delete;
//};
//
//template<>
//class Codec<char> {
//
//public:
//
//    static void serialize(char const & value, Buffer & buffer) {
//        buffer.write<std::byte>(std::byte(value));
//    }
//
//    static char deserialize(Buffer & buffer) {
//        return std::to_integer<char>(buffer.read<std::byte>());
//    }
//
//    Codec() = delete;
//};
//
//template<>
//class Codec<std::string> {
//
//public:
//
//    static void serialize(std::string const & value, Buffer & buffer) {
//        for (char character : value) {
//            buffer.write<char>(character);
//        }
//        buffer.write<char>('\0');
//    }
//
//    static std::string deserialize(Buffer & buffer) {
//        std::string value;
//        char character;
//        while ((character = buffer.read<char>()) != '\0') {
//            value.push_back(character);
//        }
//        return value;
//    }
//
//    Codec() = delete;
//};

//#include <array>
//#include <cstddef>
//#include <iostream>
//#include <stdexcept>
//
//template<std::size_t CAPACITY>
//class Buffer {
//
//    std::array<std::byte, 2 * CAPACITY> elements;
//    std::size_t head;
//    std::size_t tail;
//
//public:
//
//    explicit Buffer() : elements(), head(0), tail(0) {}
//
//    std::byte read() {
//        return std::move(elements[head = (head + 1) % CAPACITY]);
//    }
//
//    void write(std::byte const & element) {
//        elements[tail] = element;
//        elements[tail + CAPACITY] = element;
//        tail = (tail + 1) % CAPACITY;
//    }
//
//    void clear() {
//        head = 0;
//        tail = 0;
//    }
//
//    [[nodiscard]] auto begin() const {
//        auto iterator = std::begin(elements);
//        std::advance(iterator, head);
//        return iterator;
//    }
//
//    [[nodiscard]] auto end() const {
//        auto iterator = std::begin(elements);
//        if (tail >= head) {
//            std::advance(iterator, tail);
//        } else {
//            std::advance(iterator, tail + CAPACITY);
//        }
//        return iterator;
//    }
//
//    [[nodiscard]] auto begin() {
//        auto iterator = std::begin(elements);
//        std::advance(iterator, head);
//        return iterator;
//    }
//
//    [[nodiscard]] auto end() {
//        auto iterator = std::begin(elements);
//        if (tail >= head) {
//            std::advance(iterator, tail);
//        } else {
//            std::advance(iterator, tail + CAPACITY);
//        }
//        return iterator;
//    }
//
//    [[nodiscard]] std::byte * read_data() {
//        return elements.data() + head;
//    }
//
//    [[nodiscard]] std::size_t read_size() const {
//        if (tail >= head) {
//            return tail - head;
//        } else {
//            return (tail + CAPACITY) - head;
//        }
//    }
//};
//
//int main() {
//
//
//    Buffer<7> buffer;
//
//    std::cout << "Hello, World!" << std::endl;

//    Nexus::Thread server_thread([](){
//        try {
//
//            Nexus::Buffer buffer;
//
//            Nexus::ServerSocket server_socket = Nexus::ServerSocket::open(2302);
//            Nexus::Socket client_socket = server_socket.accept();
//            client_socket.read(buffer);
//
//            std::cout << buffer.read<std::string>() << std::endl;
//            std::cout << buffer.read<std::string>() << std::endl;
//            std::cout << buffer.read<int>() << std::endl;
//
//        } catch (Nexus::Exception const & exception) {
//            std::cerr << exception.what() << std::endl;
//        }
//    });
//
//    Nexus::Thread client_thread([](){
//        try {
//
//            Nexus::Buffer buffer;
//            buffer.write<std::string>("Hello, Nexus!");
//            buffer.write<std::string>("Hello!");
//            buffer.write<int>(2302);
//
//            Nexus::Socket socket = Nexus::Socket::connect("127.0.0.1", 2302);
//            socket.write(buffer);
//
//        } catch (Nexus::Exception const & exception) {
//            std::cerr << exception.what() << std::endl;
//        }
//    });

//    return 0;
//}
