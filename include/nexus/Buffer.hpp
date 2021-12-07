#pragma once

#include <stdexcept>
#include <vector>

namespace Nexus {

    template<typename Value>
    class Codec;

    class Buffer {

        std::size_t const capacity;
        std::size_t head;
        std::size_t tail;
        std::size_t count;
        std::vector<std::byte> bytes;

    public:

        explicit Buffer(std::size_t capacity) : capacity(capacity), head(0), tail(0), count(0), bytes(capacity) {}

        [[nodiscard]] std::size_t get_size() const {
            return count;
        }

        void read(void * data, std::size_t size) {

            if (size > count) {
                throw std::runtime_error("Insufficient readable memory available");
            }

            if (size <= capacity - head) {
                auto iterator = std::begin(bytes);
                std::advance(iterator, head);
                std::copy_n(iterator, size, static_cast<std::byte *>(data));
            } else {

                auto input_iterator = std::begin(bytes);
                std::advance(input_iterator, head);
                std::copy_n(input_iterator, capacity - head, static_cast<std::byte *>(data));

                auto output_iterator = static_cast<std::byte *>(data);
                std::advance(output_iterator, capacity - head);
                std::copy_n(std::begin(bytes), size - (capacity - tail), output_iterator);
            }

            head = (head + size) % capacity;
            count -= size;
        }

        void write(void const * data, std::size_t size) {

            if (size > capacity - count) {
                throw std::runtime_error("Insufficient writable memory available");
            }

            if (size <= capacity - tail) {
                auto iterator = std::begin(bytes);
                std::advance(iterator, tail);
                std::copy_n(static_cast<std::byte const *>(data), size, iterator);
            } else {

                auto output_iterator = std::begin(bytes);
                std::advance(output_iterator, tail);
                std::copy_n(static_cast<std::byte const *>(data), capacity - tail, output_iterator);

                auto input_iterator = static_cast<std::byte const *>(data);
                std::advance(input_iterator, capacity - tail);
                std::copy_n(input_iterator, size - (capacity - tail), std::begin(bytes));
            }

            tail = (tail + size) % capacity;
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
}