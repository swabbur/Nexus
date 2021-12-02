#include "WinSock.hpp"

#include <nexus/ServerSocket.hpp>
#include <iostream>
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
                std::cerr << "Could not start Windows Sockets (" << started << ')' << std::endl;
                std::exit(1);
            }

            if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2) {
                std::cerr << "Could not find a suitable version of Windows Sockets" << std::endl;
                std::exit(1);
            }
        }
    }

    void shutdown() {
        std::scoped_lock<std::mutex> lock(mutex);
        if (--count == 0) {
            int cleaned = WSACleanup();
            if (cleaned == SOCKET_ERROR) {
                int error = WSAGetLastError();
                std::cerr << "Could not clean up Windows Sockets (" << error << ')' << std::endl;
                std::exit(1);
            }
        }
    }
}
