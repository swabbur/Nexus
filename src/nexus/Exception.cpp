#include "WinSock.hpp"

#include <nexus/Exception.hpp>

namespace Nexus {

    static std::string get_message(std::int32_t code) {
        LPTSTR buffer = nullptr;
        int size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buffer, 0, nullptr);
        if (buffer != nullptr) {
            std::string message(buffer, size);
            LocalFree(buffer);
            return message;
        }
        return "An unknown error occurred.";
    }

    Exception::Exception(std::string const & message) : std::exception(message.c_str()) {}

    Exception::Exception(std::string const & prefix, std::int32_t code) : Exception(prefix + get_message(code)) {}
}
