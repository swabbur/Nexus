#include "tcp/WinSock.hpp"
#include "nexus/Exception.hpp"

#include <nexus/Selector.hpp>
#include "nexus/tcp/ServerSocket.hpp"
#include "nexus/tcp/Socket.hpp"

// TODO: Implement selector according to:
//  https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/Socket_cpp/index

namespace Nexus {

    static thread_local fd_set file_descriptors;

    Selector::Selector() : accept_callbacks(), read_callbacks() {}

    Selector::~Selector() = default;

    void Selector::attach(ServerSocket const & server_socket, std::function<void()> accept_callback) {
        accept_callbacks[&server_socket] = std::move(accept_callback);
    }

    void Selector::attach(Socket const & socket, std::function<void()> read_callback) {
        read_callbacks[&socket] = std::move(read_callback);
    }

    void Selector::detach(ServerSocket const & server_socket) {
        accept_callbacks.erase(&server_socket);
    }

    void Selector::detach(Socket const & socket) {
        read_callbacks.erase(&socket);
    }

    void Selector::poll() {

        std::unique_lock<std::mutex> lock(mutex);
        {
            FD_ZERO(&file_descriptors);

            for (auto const & [server_socket_pointer, accept_callback] : accept_callbacks) {
                FD_SET(std::any_cast<SOCKET>(server_socket_pointer->handle), &file_descriptors);
            }

            for (auto const & [socket_pointer, read_callback] : read_callbacks) {
                FD_SET(std::any_cast<SOCKET>(socket_pointer->handle), &file_descriptors);
            }
        }
        lock.unlock();

        int selected = ::select(0, &file_descriptors, nullptr, nullptr, nullptr);
        if (selected == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not select socket: ", error);
        }

        for (auto const & [server_socket_pointer, accept_callback] : accept_callbacks) {
            if (FD_ISSET(std::any_cast<SOCKET>(server_socket_pointer->handle), &file_descriptors)) {
               accept_callback();
            }
        }

        for (auto const & [socket_pointer, read_callback] : read_callbacks) {
            if (FD_ISSET(std::any_cast<SOCKET>(socket_pointer->handle), &file_descriptors)) {
                read_callback();
            }
        }
    }
}