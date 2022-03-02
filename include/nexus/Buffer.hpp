#pragma once

#include <vector>

namespace Nexus {

    template<typename Value>
    class Codec;

    class Buffer {

        friend class Socket;

        std::size_t const capacity;
        std::size_t head;
        std::size_t tail;
        std::size_t count;
        std::vector<std::byte> bytes;

    public:

        explicit Buffer(std::size_t capacity);

        [[nodiscard]] std::size_t read_available() const;

        [[nodiscard]] std::size_t write_available() const;

        void read(void * data, std::size_t size);

        void write(void const * data, std::size_t size);

        template<typename Value>
        Value read() {
            return Codec<Value>::decode(*this);
        }

        template<typename Value>
        void write(Value const & value) {
            Codec<Value>::encode(value, *this);
        }
    };
}
