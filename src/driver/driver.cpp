#include "driver/driver.h"
#include <cstdio>
#include <ctime>
#include <mutex>
#include <stop_token>
#include <unistd.h>

constexpr u_int64_t cleanup_delay{static_cast<u_int64_t>(1e6)}; // 1 second in microseconds

namespace Driver {
Driver::Driver(const std::stop_source &stop_) : main_stop{stop_} {
    garbage_collector = std::thread([&]() { CleanUp(main_stop.get_token()); });
}
Driver::~Driver() {
    garbage_collector.join();
}

void Driver::Update(int value) {
    std::scoped_lock lock(update_mutex); // TODO: Remove redundant lock

    if (value) {
        Activate();
    } else {
        PopWorker();
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
void Driver::SetBurstDelay(int64_t usec) {
    burst_delay = usec;
}

void Driver::ButtonDown() {
    std::scoped_lock lock{button_mutex};
    if (!down) {
        down = true;
        EmitDown();
    }
}

void Driver::ButtonUp() {
    std::scoped_lock lock{button_mutex};
    if (down) {
        down = false;
        EmitUp();
    }
}

void Driver::Sleep(u_int64_t usec) const {
    struct timespec ts;
    constexpr u_int64_t second = static_cast<int64_t>(1e6);
    ts.tv_sec = usec / second;
    ts.tv_nsec = usec % second * 1000;
    nanosleep(&ts, nullptr);
}

std::thread Driver::Click(std::stop_token &stoken) {
    return std::thread([&]() {
        ButtonDown();
        Sleep(hold_time);
        if (!stoken.stop_requested()) {
            ButtonUp();
        }
    });
}

void Driver::CleanUp(std::stop_token stoken) {
    auto do_cleanup = [&]() {
        while (!garbage.empty()) {
            auto &worker = garbage.front().second;

            worker.join();

            garbage.pop();
        }
    };
    while (!stoken.stop_requested()) {
        do_cleanup();
        Sleep(cleanup_delay);
    }
    do_cleanup();
}

void Driver::PopWorker() {
    std::scoped_lock lock{transfer_mutex};
    stop_sources.front().request_stop();
    ButtonUp();
    garbage.push(std::pair<std::stop_source, std::thread>{std::move(stop_sources.front()),
                                                          std::move(workers.front())});
    stop_sources.pop();
    workers.pop();
}

void Driver::Activate() {
    std::scoped_lock lock{transfer_mutex};
    stop_sources.push({});
    workers.push(std::thread{[&]() { Worker(stop_sources.back().get_token()); }});
}

void Driver::Worker(std::stop_token stoken) {
    u_int32_t burst_count{};
    while (!stoken.stop_requested() && burst_count < burst_length) {
        std::thread click;
        {
            std::scoped_lock lock{active_mutex};

            click = Click(stoken);
        }

        burst_count++;
        if (burst_count < burst_length) {
            Sleep(delay);
        } else if (burst_delay > 0) {
            burst_count = 0;
            Sleep(burst_delay);
        }

        click.join();
    }
}

} // namespace Driver