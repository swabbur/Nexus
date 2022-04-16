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

    void Selector::attach(ServerSocket const & server_socket, const std::function<void()>& accept_callback) {
        auto handle = server_socket.get_handle();
        accept_callbacks[handle] = accept_callback;
    }

    void Selector::attach(Socket const & socket, std::function<void()> const & read_callback) {
        auto handle = socket.get_handle();
        read_callbacks[handle] = read_callback;
    }

    void Selector::detach(ServerSocket const & server_socket) {
        auto handle = server_socket.get_handle();
        accept_callbacks.erase(handle);
    }

    void Selector::detach(Socket const & socket) {
        auto handle = socket.get_handle();
        read_callbacks.erase(handle);
    }

    void Selector::poll() {

        std::unique_lock<std::mutex> lock(mutex);
        {
            FD_ZERO(&file_descriptors);

            for (auto const & [handle, accept_callback] : accept_callbacks) {
                FD_SET(handle, &file_descriptors);
            }

            for (auto const & [handle, read_callback] : read_callbacks) {
                FD_SET(handle, &file_descriptors);
            }
        }
        lock.unlock();

        int selected = ::select(0, &file_descriptors, nullptr, nullptr, nullptr);
        if (selected == SOCKET_ERROR) {
            int error = WSAGetLastError();
            throw Exception("Could not select socket: ", error);
        }

        for (auto const & [handle, accept_callback] : accept_callbacks) {
            if (FD_ISSET(handle, &file_descriptors)) {
               accept_callback();
            }
        }

        for (auto const & [handle, read_callback] : read_callbacks) {
            if (FD_ISSET(handle, &file_descriptors)) {
                read_callback();
            }
        }
    }
}