#include "driver/driver.h"
#include "driver/uinput.h"
#include "watcher/buttons.h"
#include "watcher/event.h"
#include "watcher/watcher.h"
#include <getopt.h>
#include <memory>
#include <set>
#include <stop_token>
#include <unistd.h>

constexpr struct option longopts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"burst", required_argument, nullptr, 'b'},
    {"rate", required_argument, nullptr, 'r'},
    {"delay", required_argument, nullptr, 'd'},
    {"hold-delay", required_argument, nullptr, 'o'},
    {"key-bind", required_argument, nullptr, 'k'},
    {nullptr, 0, nullptr, 0},
};

constexpr char const *optstring = "hb:r:d:o:k:"; // TODO: process watch

void Worker(Watcher::Watcher &watcher, Driver::Driver &driver,
            const std::set<Watcher::Button> &binds, std::stop_token stoken) {
    Watcher::Button button;
    int value;
    while (!stoken.stop_requested()) {
        watcher.ReadInput(button, value);
        if (binds.contains(button)) {
            driver.Update(value);
        }
    }
}

int main(int argc, char *argv[]) {
    const char *filename;
    int c;

    int burst_length{};
    int rate{};
    int delay{};
    int hold_delay{};
    std::set<Watcher::Button> key_binds{};

    while ((c = getopt_long(argc, argv, optstring, longopts, nullptr)) != -1) {
        switch (c) {
        case 'h':
            return -1;
        case 'b':
            burst_length = atoi(optarg);
            break;
        case 'r':
            rate = atoi(optarg);
            break;
        case 'd':
            delay = atoi(optarg);
            break;
        case 'o':
            hold_delay = atoi(optarg);
            break;
        case 'k': {
            auto value = Watcher::ToEnum<Watcher::Button>(optarg);
            key_binds.insert(value);
            break;
        }
        case '?':
        case ':':
            return -1;
        default:
            printf("%s\n", optarg);
        }
    }

    if (optind < argc) {
        filename = argv[optind];
    } else {
        return -1;
    }

    if (rate != 0) {
        delay = 60.0f / static_cast<double>(rate) * 1000000.0;
        printf("Delay set to %.03lf ms\n", delay / 1000.0);
    } else if (delay != 0) {
        delay *= 1000;
    }
    if (hold_delay != 0) {
        hold_delay *= 1000;
    }

    if (hold_delay >= delay) {
        printf("hold delay must be less than delay\n");
        return -1;
    }

    if (key_binds.empty()) {
        key_binds.insert(Watcher::Button::Middle);
    }

    // Start the system
    std::unique_ptr<Driver::Driver> driver = std::make_unique<Driver::Uinput>();
    std::unique_ptr<Watcher::Watcher> watcher = std::make_unique<Watcher::EventWatcher>(filename);

    if (delay != 0) {
        driver->SetDelay(delay);
    }
    if (hold_delay != 0) {
        driver->SetHoldTime(hold_delay);
    }
    if (burst_length != 0) {
        driver->SetBurstLength(burst_length);
    }

    // Driver must be started before the key watcher
    driver->Start();

    std::stop_source stop;
    std::thread thread(
        [&](std::stop_token s) { Worker(*watcher.get(), *driver.get(), key_binds, s); },
        stop.get_token());
    thread.join();

    return 0;
}