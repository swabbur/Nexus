#pragma once

#include <queue>

namespace Nexus {

    class Buffer {

        std::queue<std::byte> queue;

    public:

        explicit Buffer();

        std::vector<std::byte> read_all() {
            std::vector<std::byte> bytes;
            while (!queue.empty()) {
                bytes.push_back(queue.front());
                queue.pop();
            }
            return bytes;
        }

        template<typename Value>
        Value read() {
            return Codec<Value>::deserialize(queue);
        }

        template<typename Value>
        void write(Value const & value) {
            Codec<Value>::serialize(value, queue);
        }
    };
}

#include "Codec.hpp"
