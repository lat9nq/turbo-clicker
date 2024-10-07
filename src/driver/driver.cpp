#include "driver/driver.h"
#include <mutex>
#include <stop_token>
#include <unistd.h>

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

std::thread Driver::Click(std::stop_token &stoken) {
    return std::thread([&]() {
        ButtonDown();
        usleep(hold_time);
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
        usleep(1000000);
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
    workers.push(std::thread{[&]() { Worker(stop_sources.front().get_token()); }});
}

void Driver::Worker(std::stop_token stoken) {
    for (u_int32_t burst_count = 0; burst_count < burst_length; burst_count++) {
        std::thread click;
        {
            std::scoped_lock lock{active_mutex};
            if (stoken.stop_requested()) {
                break;
            }

            click = Click(stoken);
            burst_count++;
        }

        usleep(delay);
        click.join();
    }
}

} // namespace Driver