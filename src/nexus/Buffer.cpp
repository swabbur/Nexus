#include <nexus/Buffer.hpp>

namespace Nexus {

    Buffer::Buffer(std::size_t initial_capacity) : bytes(initial_capacity, std::byte(0)), position(0) {}

    Buffer::Buffer(Buffer const & buffer) : bytes(buffer.bytes.capacity()), position(buffer.position) {
        std::move(std::begin(buffer.bytes), std::end(buffer.bytes), std::begin(bytes));
    }

    Buffer::Buffer(Buffer && buffer) noexcept : bytes(std::move(buffer.bytes)), position(buffer.position) {}

    Buffer::~Buffer() = default;
}