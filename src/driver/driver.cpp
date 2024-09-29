#include "driver/driver.h"
#include <stop_token>
#include <unistd.h>

namespace Driver {
Driver::Driver() {
    mutex.lock();
};
Driver::~Driver() = default;

void Driver::Start() {
    thread =
        std::make_unique<std::jthread>([&](std::stop_token s) { Worker(s); }, stop.get_token());
}

void Driver::Update(int value) {
    if (value == 0) {
        mutex.lock();
    } else {
        burst_count = 0;
        mutex.unlock();
    }
}

void Driver::SetDelay(u_int64_t usec) {
    delay = usec;
}
void Driver::SetHoldTime(u_int64_t usec) {
    hold_time = usec;
}
void Driver::SetBurstLength(u_int32_t length) {
    burst_length = length;
}

void Driver::Join() {
    thread->join();
}

void Driver::Worker(std::stop_token stoken) {
    const auto click = [&]() {
        ButtonDown();
        usleep(hold_time);
        ButtonUp();
    };

    while (!stoken.stop_requested()) {
        mutex.lock();

        if (burst_length == 0) {
            click();
        } else if (burst_count < burst_length) {
            click();
        }
        burst_count++;

        mutex.unlock();

        usleep(delay - hold_time);
    }
}

} // namespace Driver