#pragma once

#include <cstdint>
#include <queue>
#include <string>
#include <vector>

namespace Nexus {

    template<typename Value>
    class Codec;

    template<typename Value>
    class Codec {

    public:

        static void serialize(Value const & value, std::queue<std::byte> & queue) {
            auto const * bytes = reinterpret_cast<std::byte const *>(&value);
            for (std::size_t index = 0; index < sizeof(Value); index++) {
                queue.push(bytes[index]);
            }
        }

        static Value deserialize(std::queue<std::byte> & queue) {
            std::byte bytes[sizeof(Value)];
            for (std::size_t index = 0; index < sizeof(Value); index++) {
                bytes[index] = queue.front();
                queue.pop();
            }
            return *reinterpret_cast<Value *>(bytes);
        }

        Codec() = delete;
    };

    template<>
    class Codec<std::string> {

    public:

        static void serialize(std::string const & value, std::queue<std::byte> & queue) {
            for (char character : value) {
                queue.push(std::byte(character));
            }
            queue.push(std::byte(0));
        }

        static std::string deserialize(std::queue<std::byte> & queue) {
            std::string value;
            while (queue.front() != std::byte(0)) {
                value.push_back(std::to_integer<char>(queue.front()));
                queue.pop();
            }
            queue.pop();
            return value;
        }

        Codec() = delete;
    };

    template<typename Value>
    class Codec<std::vector<Value>> {

    public:

        static void serialize(std::vector<Value> const & value, std::queue<std::byte> & queue) {
            Codec<std::size_t>::serialize(value.size(), queue);
            for (Value const & sub_value : value) {
                Codec<Value>::serialize(sub_value, queue);
            }
        }

        static std::vector<Value> deserialize(std::queue<std::byte> & queue) {
            std::vector<Value> value;
            std::size_t size = Codec<std::size_t>::deserialize(queue);
            value.reserve(size);
            for (std::size_t index = 0; index < size; index++) {
                Value sub_value = Codec<Value>::deserialize(queue);
                value.push_back(std::move(sub_value));
            }
            return value;
        }

        Codec() = delete;
    };

    template<>
    class Codec<char> {

    public:

        static void serialize(char const & value, std::queue<std::byte> & queue) {
            queue.push(std::byte(value));
        }

        static char deserialize(std::queue<std::byte> & queue) {
            char value = std::to_integer<char>(queue.front());
            queue.pop();
            return value;
        }

        Codec() = delete;
    };
}
