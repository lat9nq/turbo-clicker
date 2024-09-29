#include "watcher/watcher.h"

#include <stop_token>
#include <thread>

namespace Watcher {
Watcher::Watcher() = default;
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
        printf("button %d value %d\n", static_cast<int>(button), value);

        if (button == Button::Middle) {
            // Driver.Down();
        }
    }
}

void Watcher::Join() {
    thread->join();
}
} // namespace Watcher