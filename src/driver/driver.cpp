#include "driver/driver.h"
#include <mutex>
#include <stop_token>
#include <unistd.h>

namespace Driver {
Driver::Driver(const std::stop_source &stop_) : stop{stop_} {
    mutex.lock();
};
Driver::~Driver() = default;

void Driver::Start() {
    thread =
        std::make_unique<std::jthread>([&](std::stop_token s) { Worker(s); }, stop.get_token());
}

void Driver::Update(int value) {
    std::scoped_lock lock(update_mutex);
    static u_int32_t active{0};

    if (value) {
        active++;
        if (active == 1) {
            burst_count = 0;
            mutex.unlock(); // start
        }
    } else {
        active--;
        if (active == 0) {
            mutex.lock(); // stop
        }
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

    while (1) {
        mutex.lock();
        if (stoken.stop_requested()) {
            break;
        }

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