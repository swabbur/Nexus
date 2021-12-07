#pragma once

#include "Buffer.hpp"

#include <array>
#include <type_traits>

namespace Nexus {

    template<typename Value>
    class Codec {

        static_assert(std::is_move_constructible_v<Value> && std::is_trivially_copyable_v<Value>,
                      "The default codec is only guaranteed to work on types that are both move-constructible and trivially "
                      "copyable");

    public:

        static void serialize(Value const & value, Buffer & buffer) {
            buffer.write(&value, sizeof(Value));
        }

        static Value deserialize(Buffer & buffer) {
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

        static void serialize(std::array<Value, SIZE> const & values, Buffer & buffer) {
            for (Value const & value : values) {
                Codec<Value>::serialize(value, buffer);
            }
        }

        static std::array<Value, SIZE> deserialize(Buffer & buffer) {
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

        static void serialize(std::vector<Value> const & values, Buffer & buffer) {
            Codec<std::size_t>::serialize(values.size(), buffer);
            for (Value const & value : values) {
                Codec<Value>::serialize(value, buffer);
            }
        }

        static std::vector<Value> deserialize(Buffer & buffer) {
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

        static void serialize(std::string const & value, Buffer & buffer) {
            buffer.write(value.c_str(), value.size() + 1);
        }

        static std::string deserialize(Buffer & buffer) {
            std::string value;
            char character;
            while ((character = Codec<char>::deserialize(buffer)) != '\0') {
                value += character;
            }
            return value;
        }

        Codec() = delete;
    };
}
