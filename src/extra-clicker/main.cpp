#include "driver/driver.h"
#include "driver/uinput.h"
#include "watcher/buttons.h"
#include "watcher/event.h"
#include "watcher/watcher.h"
#include <argp.h>
#include <bits/types/error_t.h>
#include <cerrno>
#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <memory>
#include <set>
#include <stop_token>
#include <sys/types.h>
#include <unistd.h>

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

struct Settings {
  public:
    explicit Settings() = default;
    ~Settings() = default;

    int burst_length{};
    int rate{};
    int delay{};
    int hold_delay{};
    std::set<Watcher::Button> key_binds{};
};

error_t Parser(int key, char *arg, struct argp_state *state) {
    struct Settings &settings = *reinterpret_cast<struct Settings *>(state->input);
    switch (key) {
    case 'b':
        settings.burst_length = atoi(arg);
        break;
    case 'r':
        settings.rate = atoi(arg);
        break;
    case 'd':
        settings.delay = atoi(arg);
        break;
    case 'o':
        settings.hold_delay = atoi(arg);
        break;
    case 'k': {
        auto value = Watcher::ToEnum<Watcher::Button>(arg);
        settings.key_binds.insert(value);
        std::printf("Bound %s to clicker\n", Watcher::CanonicalizeEnum(value).c_str());
        break;
    }
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return {};
}

constexpr struct argp_option options[] = {
    {"burst", 'b', "count", 0, "Adds a burst configuration (0 = no limit) (default 0)", 0},
    {"delay", 'd', "ms", 0, "Add a delay between clicks (default 66ms)", 0},
    {"rate", 'r', "cpm", 0, "Add a delay specified in clicks per minute (default 900 cpm)", 0},
    {"key-bind", 'k', "button", 0, "Bind a mouse button to this program (default Middle)", 0},
    {"hold-delay", 'o', "ms", 0, "Specify the amount of time to hold the click down (default 20ms)",
     0},
    // Null terminator
    {nullptr, 0, nullptr, 0, nullptr, 0},
};

constexpr char const *doc = nullptr;

constexpr struct argp argp_data {
    options, Parser, 0, doc, nullptr, nullptr, nullptr
};

int main(int argc, char *argv[]) {
    std::vector<int> descriptors;
    struct Settings settings {};

    argp_parse(&argp_data, argc, argv, 0, nullptr, &settings);

    if (settings.rate != 0) {
        settings.delay = 60.0f / static_cast<double>(settings.rate) * 1000000.0;
        std::printf("Delay set to %.03lf ms\n", settings.delay / 1000.0);
    } else if (settings.delay != 0) {
        std::printf("Delay set to %d ms\n", settings.delay);
        settings.delay *= 1000;
    }
    if (settings.hold_delay != 0) {
        std::printf("Hold delay set to %d ms\n", settings.hold_delay);
        settings.hold_delay *= 1000;

        // TODO: Sanitize hold_delay > delay
    }
    if (settings.key_binds.empty()) {
        settings.key_binds.insert(Watcher::Button::Middle);
    }

    // if (optind < argc) {
    //     descriptors.push_back(open(argv[optind], O_RDONLY));
    //     if (descriptors.back() == -1) {
    //         int err = errno;
    //         std::fprintf(stderr, "error %d\n", err);
    //         return -1;
    //     }
    // } else {
    int dirfd = open("/dev/input/by-id", O_RDONLY);
    if (dirfd == -1) {
        int err = errno;
        std::fprintf(stderr, "error %d\n", err);
        return -1;
    }
    DIR *dir_stream = fdopendir(dirfd);
    if (dir_stream == nullptr) {
        int err = errno;
        std::fprintf(stderr, "error %d\n", err);
        return -1;
    }
    struct dirent *entry;
    std::vector<std::string> device_names;
    while ((entry = readdir(dir_stream)) != nullptr) {
        std::string filename{entry->d_name};
        if (filename.find("event-mouse") == std::string::npos ||
            filename.find("if01") != std::string::npos) {
            continue;
        }
        device_names.push_back(filename);
        descriptors.push_back(openat(dirfd, entry->d_name, O_RDONLY));
    }
    printf("Found %ld device%s\n", descriptors.size(), descriptors.size() != 1 ? "s" : "");
    for (const auto &name : device_names) {
        std::printf("- %s\n", name.c_str());
    }
    // }

    // Start the system
    std::unique_ptr<Driver::Driver> driver = std::make_unique<Driver::Uinput>();
    std::vector<std::unique_ptr<Watcher::Watcher>> watchers;
    for (const auto fd : descriptors) {
        watchers.push_back(std::make_unique<Watcher::EventWatcher>(fd));
    }

    if (settings.delay != 0) {
        driver->SetDelay(settings.delay);
    }
    if (settings.hold_delay != 0) {
        driver->SetHoldTime(settings.hold_delay);
    }
    if (settings.burst_length != 0) {
        driver->SetBurstLength(settings.burst_length);
    }

    // Driver must be started before the key watcher
    driver->Start();

    std::stop_source stop;
    std::vector<std::thread> threads;
    for (const auto &watcher : watchers) {
        threads.push_back(std::thread(
            [&](std::stop_token s) {
                Worker(*watcher.get(), *driver.get(), settings.key_binds, s);
            },
            stop.get_token()));
    }
    for (auto &thread : threads) {
        thread.join();
    }

    return 0;
}