#include <nexus/Buffer.hpp>
#include <nexus/Codec.hpp>
#include <nexus/Nexus.hpp>
#include <nexus/Selector.hpp>
#include <nexus/ServerSocket.hpp>
#include <nexus/Socket.hpp>
#include <iostream>
#include <thread>

// TODO: Generate error messages according to:
//  https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2

int main() {
    Nexus::init();
    Nexus::ServerSocket server_socket = Nexus::open(2302);
    Nexus::Socket socket = Nexus::connect("127.0.0.1", 2302);
    Nexus::shutdown();
    return 0;
}