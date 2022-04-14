#include <catch2/catch.hpp>
#include <nexus/Buffer.hpp>
#include <array>

static constexpr std::size_t DEFAULT_CAPACITY = 1024;

TEST_CASE("construction", "[buffer]") {

    Nexus::Buffer buffer(DEFAULT_CAPACITY);

    REQUIRE(buffer.read_available() == 0);
    REQUIRE(buffer.write_available() == DEFAULT_CAPACITY);
}

TEST_CASE("reading/writing", "[buffer]") {

    Nexus::Buffer buffer(DEFAULT_CAPACITY);

    std::array<std::uint8_t, 4> written_bytes = { 1, 2, 3, 4 };
    buffer.write(&written_bytes, sizeof(written_bytes));

    REQUIRE(buffer.read_available() == sizeof(written_bytes));
    REQUIRE(buffer.write_available() == DEFAULT_CAPACITY - buffer.read_available());

    std::array<std::uint8_t, 2> read_bytes = {};
    buffer.read(&read_bytes, sizeof(read_bytes));

    REQUIRE(buffer.read_available() == sizeof(written_bytes) - sizeof(read_bytes));
    REQUIRE(buffer.write_available() == DEFAULT_CAPACITY - buffer.read_available());

    REQUIRE(std::equal(std::begin(read_bytes), std::end(read_bytes), std::begin(written_bytes)));
}
