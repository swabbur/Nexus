#include "WinSock.hpp"

#include "nexus/Exception.hpp"
#include "nexus/tcp/ServerSocket.hpp"
#include <mutex>

namespace WinSock {

    static std::mutex mutex;
    static std::size_t count;

    void init() {
        std::scoped_lock<std::mutex> lock(mutex);
        if (count++ == 0) {

            WORD version = MAKEWORD(2, 2);
            WSADATA data;
            int started = WSAStartup(version, &data);
            if (started != 0) {
                throw Nexus::Exception("Could not start Windows Sockets: ", started);
            }

            if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2) {
                throw Nexus::Exception("Could not find a suitable version of Windows Sockets");
            }
        }
    }

    void shutdown() {
        std::scoped_lock<std::mutex> lock(mutex);
        if (--count == 0) {
            int cleaned = WSACleanup();
            if (cleaned == SOCKET_ERROR) {
                int error = WSAGetLastError();
                throw Nexus::Exception("Could not clean up Windows Sockets: ", error);
            }
        }
    }
}
