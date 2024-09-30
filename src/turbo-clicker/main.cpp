#include "driver/driver.h"
#include "driver/uinput.h"
#include "input/buttons.h"
#include "input/event.h"
#include "input/input.h"
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

struct Settings {
  public:
    explicit Settings() = default;
    ~Settings() = default;

    int current_delay{0};

    std::vector<int> burst_length{};
    std::vector<double> delay{};
    std::vector<double> hold_delay{};
    std::set<Input::Button> key_binds{};
    std::set<Input::Button> rate_cycle_binds{};
};

void Worker(Input::Device &Input, Driver::Driver &driver, Settings &settings,
            std::stop_token stoken) {
    Input::Button button;
    int value;
    while (!stoken.stop_requested()) {
        Input.ReadInput(button, value);
        if (settings.key_binds.contains(button)) {
            driver.Update(value);
        }
        if (settings.rate_cycle_binds.contains(button) && value == 1) {
            settings.current_delay = (settings.current_delay + 1) % settings.delay.size();
            double new_delay = settings.delay[settings.current_delay];
            driver.SetDelay(new_delay * 1000.0);
            std::printf("Set delay to #%d %.03fms [%.03f cpm]\n", settings.current_delay, new_delay,
                        1.0 / (new_delay / 1000.0 / 60.0));
        }
    }
}

constexpr int cycle_rate_key{1};

error_t Parser(int key, char *arg, struct argp_state *state) {
    struct Settings &settings = *reinterpret_cast<struct Settings *>(state->input);
    switch (key) {
    case 'b':
        settings.burst_length.push_back(atoi(arg));
        break;
    case 'r':
        settings.delay.push_back(60000.0f / std::atof(arg));
        break;
    case 'd':
        settings.delay.push_back(atof(arg));
        break;
    case 'o':
        settings.hold_delay.push_back(atof(arg));
        break;
    case 'k': {
        auto value = Input::ToEnum<Input::Button>(arg);
        settings.key_binds.insert(value);
        std::printf("Bound %s to clicker\n", Input::CanonicalizeEnum(value).c_str());
        break;
    }
    case cycle_rate_key: {
        auto value = Input::ToEnum<Input::Button>(arg);
        settings.rate_cycle_binds.insert(value);
        std::printf("Bound %s to cycle rates\n", Input::CanonicalizeEnum(value).c_str());
    } break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return {};
}

constexpr struct argp_option options[] = {
    {"burst", 'b', "count", 0, "Adds a burst configuration (0 = no limit) (default 0)", 0},
    {"delay", 'd', "ms", 0, "Add a delay between clicks (default 66ms)", 0},
    {"rate", 'r', "cpm", 0, "Add a delay specified in clicks per minute (default 900 cpm)", 0},
    {"key-bind", 'k', "button", 0, "Bind a mouse button to activate clicks (default Middle)", 0},
    {"hold-delay", 'o', "ms", 0, "Specify the amount of time to hold the click down (default 20ms)",
     0},
    {"cycle-rate", cycle_rate_key, "button", 0, "Bind a button to cycle different rates", 1},
    // Null terminator
    {nullptr, 0, nullptr, 0, nullptr, 0},
};

constexpr char const *doc =
    "Watches input devices events to create repeated simulated mouse clicks."
    "\vAdditional rates and delays can be specified, e.g. \"-r 900 -r 1100\". To make use of "
    "additional rates, a cycle bind must be specified using \"--cycle-rate\".";

constexpr struct argp argp_data {
    options, Parser, 0, doc, nullptr, nullptr, nullptr
};

int main(int argc, char *argv[]) {
    std::vector<int> descriptors;
    struct Settings settings {};

    argp_parse(&argp_data, argc, argv, 0, nullptr, &settings);

    if (!settings.delay.empty()) {
        std::printf("Delay set to %.03lf ms\n", settings.delay.front());
    }
    if (!settings.hold_delay.empty()) {
        std::printf("Hold delay set to %.03lf ms\n", settings.hold_delay.front());
        // TODO: Sanitize hold_delay > delay
    }
    if (settings.key_binds.empty()) {
        settings.key_binds.insert(Input::Button::Middle);
    }

    // if (optind < argc) {
    //     descriptors.push_back(open(argv[optind], O_RDONLY));
    //     if (descriptors.back() == -1) {
    //         int err = errno;
    //         std::fprintf(stderr, "error %d\n", err);
    //         return -1;
    //     }
    // } else {
    {
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
    }

    // Start the system
    std::unique_ptr<Driver::Driver> driver = std::make_unique<Driver::Uinput>();
    std::vector<std::unique_ptr<Input::Device>> Inputs;
    for (const auto fd : descriptors) {
        Inputs.push_back(std::make_unique<Input::Event>(fd));
    }

    if (!settings.delay.empty()) {
        driver->SetDelay(settings.delay.front() * 1000.0);
    }
    if (!settings.hold_delay.empty()) {
        driver->SetHoldTime(settings.hold_delay.front() * 1000.0);
    }
    if (!settings.burst_length.empty()) {
        driver->SetBurstLength(settings.burst_length.front());
    }

    // Driver must be started before the input thread
    driver->Start();

    std::stop_source stop;
    std::vector<std::thread> threads;
    for (const auto &Input : Inputs) {
        threads.push_back(std::thread(
            [&](std::stop_token s) { Worker(*Input.get(), *driver.get(), settings, s); },
            stop.get_token()));
    }
    for (auto &thread : threads) {
        thread.join();
    }

    return 0;
}