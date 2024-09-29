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
        mutex.unlock();
    }
}

void Driver::SetDelay(u_int64_t usec) {
    delay = usec;
}
void Driver::SetHoldTime(u_int64_t usec) {
    hold_time = usec;
}
void Driver::Join() {
    thread->join();
}
void Driver::Worker(std::stop_token stoken) {
    while (!stoken.stop_requested()) {
        mutex.lock();
        ButtonDown();
        usleep(hold_time);
        ButtonUp();
        mutex.unlock();
        usleep(delay - hold_time);
    }
}

} // namespace Driver