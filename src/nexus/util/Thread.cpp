#include <nexus/util/Thread.hpp>

namespace Nexus {

    Thread::Thread(std::function<void()> const & function) : thread(function) {}

    Thread::~Thread() {
        if (thread.joinable()) {
            thread.join();
        }
    }
}
