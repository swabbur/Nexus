#pragma once

#include <cstdint>
#include <vector>

namespace Nexus {

    class Buffer {

        std::vector<std::byte> bytes;
        std::size_t position;

    public:

        explicit Buffer(std::size_t initial_capacity);

        Buffer(Buffer const & buffer);

        Buffer(Buffer && buffer) noexcept;

        ~Buffer();
    };
}
