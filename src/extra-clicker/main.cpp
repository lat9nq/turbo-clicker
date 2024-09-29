#include "driver/driver.h"
#include "driver/uinput.h"
#include "watcher/event.h"
#include <getopt.h>
#include <memory>
#include <unistd.h>

int main(int argc, char *argv[]) {
    const char *filename;
    const char *optstring = "hb:r:d:o:"; // TODO: keybind, process watch
    int c;

    int burst_length{};
    int rate{};
    int delay{};
    int hold_delay{};

    const struct option longopts[] = {
        {"help", no_argument, nullptr, 'h'},
        {"burst", required_argument, nullptr, 'b'},
        {"rate", required_argument, nullptr, 'r'},
        {"delay", required_argument, nullptr, 'd'},
        {"hold-delay", required_argument, nullptr, 'o'},
        {nullptr, 0, nullptr, 0},
    };

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

    std::unique_ptr<Driver::Driver> driver = std::make_unique<Driver::Uinput>();
    std::unique_ptr<Watcher::Watcher> watcher =
        std::make_unique<Watcher::EventWatcher>(*driver.get(), filename);

    if (delay != 0) {
        driver->SetDelay(delay);
    }
    if (hold_delay != 0) {
        driver->SetHoldTime(hold_delay);
    }
    if (burst_length != 0) {
        driver->SetBurstLength(burst_length);
    }

    driver->Start();
    watcher->Start();

    return 0;
}