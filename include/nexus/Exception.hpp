#pragma once

#include <exception>
#include <string>

namespace Nexus {

    class Exception : public std::exception {

    public:

        explicit Exception(std::string const & message);

        explicit Exception(std::string const & prefix, std::int32_t code);
    };
}
