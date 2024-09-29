#include "watcher/watcher.h"
#include "driver/driver.h"

#include <stop_token>
#include <thread>

namespace Watcher {
Watcher::Watcher(Driver::Driver &driver_) : driver{driver_} {
}
Watcher::~Watcher() = default;

void Watcher::Start() {
    thread = std::make_unique<std::jthread>([&](std::stop_token s) { this->Worker(s); },
                                            stop.get_token());
}

void Watcher::Worker(std::stop_token stoken) {
    Button button;
    int value;
    while (!stoken.stop_requested()) {
        ReadInput(button, value);

        if (button == Button::Middle) {
            driver.Update(value);
        }
    }
}

void Watcher::Join() {
    thread->join();
}
} // namespace Watcher