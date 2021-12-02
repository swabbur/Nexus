#pragma once

#include <functional>
#include <string>
#include <thread>

namespace Nexus {

    class Thread {

        std::thread thread;

    public:

        explicit Thread(std::function<void()> const & function);

        Thread(Thread const & thread) = delete;

        Thread(Thread && thread) = delete;

        ~Thread();
    };
}