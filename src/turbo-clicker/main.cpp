#include "driver/driver.h"
#include "driver/uinput.h"
#include "input/buttons.h"
#include "input/event.h"
#include "input/input.h"
#include <argp.h>
#include <bits/types/error_t.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <error.h>
#include <fcntl.h>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <set>
#include <stop_token>
#include <sys/types.h>
#include <unistd.h>

struct Settings {
  public:
    explicit Settings() = default;
    ~Settings() = default;

    const char *status_file{nullptr};

    int current_delay{0};
    int current_burst{0};
    int current_hold_delay{0};

    bool toggle{};
    std::atomic<bool> toggle_on{};

    std::vector<u_int32_t> burst_length{};
    std::vector<double> burst_delay{};
    std::vector<double> delay{};
    std::vector<double> hold_delay{};
    std::set<Input::Button> key_binds{};
    std::set<Input::Button> rate_cycle_binds{};
    std::set<Input::Button> burst_cycle_binds{};
    std::set<Input::Button> hold_cycle_binds{};
    std::vector<const char *> devices{};
};

constexpr struct {
    u_int32_t burst_length{std::numeric_limits<u_int32_t>::max()};
    double delay{66.666};
    double hold_delay{6.000};
    double burst_delay{-1};
} defaults;

void WriteStatus(int status_file_fd, const Settings &settings) {
    static std::mutex status_writer_mutex;
    std::scoped_lock lock{status_writer_mutex};

    if (status_file_fd == -1) {
        return;
    }

    if (lseek(status_file_fd, 0, SEEK_SET) == -1) {
        int err = errno;
        std::fprintf(stderr, "error seeking to status file start %d\n", err);
        return;
    }
    if (ftruncate(status_file_fd, 0) == -1) {
        int err = errno;
        std::fprintf(stderr, "error truncating status file %d\n", err);
        return;
    }
    char cpm[64], burst[64], data[255];
    std::snprintf(cpm, 64, "CPM: %.00f", 60.0 * 1000.0 / settings.delay[settings.current_delay]);
    if (settings.burst_length[settings.current_burst] != defaults.burst_length ||
        settings.burst_length.size() > 1) {
        std::snprintf(burst, 64, "BURST: %d", settings.burst_length[settings.current_burst]);
        std::snprintf(data, 255, "%s\t%s", cpm, burst);
    } else {
        std::snprintf(data, 255, "%s", cpm);
    }
    if (write(status_file_fd, data, std::strlen(data)) == -1) {
        int err = errno;
        std::fprintf(stderr, "error writing status file %d\n", err);
        return;
    }
}

template <typename Type>
void Cycle(int &index, const std::vector<Type> &data, std::function<void(Type)> set) {
    index = (index + 1) % data.size();
    Type current = data.at(index);
    set(current);
};

void Worker(Input::Device &Input, Driver::Driver &driver, Settings &settings, int status_file_fd,
            std::stop_token stoken) {

    Input::Button button{Input::Button::None};
    int value{0}; // TODO: Make this an enum
    while (!stoken.stop_requested()) {
        if (settings.key_binds.contains(button)) {
            if (settings.toggle && value) {
                settings.toggle_on = !settings.toggle_on;
                driver.Update(static_cast<int>(settings.toggle_on));
            } else if (!settings.toggle) {
                driver.Update(value);
            }
        }
        if (value != 0) {
            if (settings.rate_cycle_binds.contains(button)) {
                Cycle<double>(settings.current_delay, settings.delay,
                              [&](double x) { driver.SetDelay(x * 1000.0); });
                double delay = settings.delay[settings.current_delay];
                std::printf("Set delay to #%d: %.03fms [%.03f cpm]\n", settings.current_delay,
                            delay, 1.0 / (delay / 1000.0 / 60.0));
            }
            if (settings.burst_cycle_binds.contains(button)) {
                Cycle<u_int32_t>(settings.current_burst, settings.burst_length,
                                 [&](int x) { driver.SetBurstLength(x); });
                std::printf("Set burst length to #%d: %d\n", settings.current_burst,
                            settings.burst_length[settings.current_burst]);
            }
            if (settings.hold_cycle_binds.contains(button)) {
                Cycle<double>(settings.current_hold_delay, settings.hold_delay,
                              [&](double x) { driver.SetHoldTime(x * 1000.0); });
                std::printf("Set hold delay to #%d: %.03fms\n", settings.current_hold_delay,
                            settings.hold_delay[settings.current_hold_delay]);
            }
        }

        // if (button != Input::Button::None) {
        //     std::printf("%02d %s\n", value,
        //     Input::CanonicalizeEnum<Input::Button>(button).c_str());
        // }

        WriteStatus(status_file_fd, settings);

        Input.ReadInput(button, value);
    }
}

enum {
    cycle_rate_key = 1,
    cycle_burst_key,
    cycle_hold_delay_key,
    burst_delay_key,
};

struct HandlerData {
    int status_file_fd;
    const char *status_file;
    std::stop_source &stop;
    Driver::Driver &driver;
};
static void *handler_input{nullptr};
extern "C" void Handler(int signal) {
    struct HandlerData &data = *reinterpret_cast<struct HandlerData *>(handler_input);
    std::fprintf(stderr, "%s received, stopping...\n", strsignal(signal));
    close(data.status_file_fd);
    unlink(data.status_file);
    data.stop.request_stop();
}

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
    case 't': {
        settings.toggle = true;
        break;
    }
    case burst_delay_key:
        settings.burst_delay.push_back(atof(arg));
        std::printf("Set burst delay to %.0f ms\n", settings.burst_delay.back());
        break;
    case cycle_rate_key: {
        auto value = Input::ToEnum<Input::Button>(arg);
        settings.rate_cycle_binds.insert(value);
        std::printf("Bound %s to cycle rates\n", Input::CanonicalizeEnum(value).c_str());
    } break;
    case cycle_burst_key: {
        auto value = Input::ToEnum<Input::Button>(arg);
        settings.burst_cycle_binds.insert(value);
        std::printf("Bound %s to cycle burst lengths\n", Input::CanonicalizeEnum(value).c_str());
    } break;
    case cycle_hold_delay_key: {
        auto value = Input::ToEnum<Input::Button>(arg);
        settings.hold_cycle_binds.insert(value);
        std::printf("Bound %s to cycle hold delays\n", Input::CanonicalizeEnum(value).c_str());
    } break;
    case 's':
        if (arg == nullptr) {
            settings.status_file = "/tmp/status";
        } else {
            settings.status_file = arg;
        }
        break;
    case 'f':
        settings.devices.push_back(arg);
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return {};
}

constexpr struct argp_option options[] = {
    {"burst", 'b', "count", 0, "Adds a burst configuration (0 = no limit) (default 0)", 0},
    {"burst-delay", burst_delay_key, "ms", 0, "Enable repeat bursts with the specified delay", 0},
    {"delay", 'd', "ms", 0, "Add a delay between clicks (default 66ms)", 0},
    {"rate", 'r', "cpm", 0, "Add a delay specified in clicks per minute (default 900 cpm)", 0},
    {"key-bind", 'k', "button", 0, "Bind a mouse button to activate clicks (default Middle)", 0},
    {"hold-delay", 'o', "ms", 0, "Specify the amount of time to hold the click down (default 20ms)",
     0},
    {"toggle", 't', nullptr, 0,
     "Activate clicker with a toggle as opposed to hold-press (default Off)", 0},
    {"cycle-rate", cycle_rate_key, "button", 0, "Bind a button to cycle different rates", 1},
    {"cycle-burst", cycle_burst_key, "button", 0, "Bind a button to cycle different burst lengths",
     1},
    {"cycle-hold-delay", cycle_hold_delay_key, "button", 0, "Bind a button to cycle hold delays",
     1},
    {"status-file", 's', "filename", OPTION_ARG_OPTIONAL,
     "Location to live update the status of the program", 2},
    {"file-device", 'f', "filenaeme", 0, "Specify the /dev/input device to use", 3},
    // Null terminator
    {nullptr, 0, nullptr, 0, nullptr, 0},
};

constexpr char const *doc =
    "Watches input devices events to create repeated simulated mouse clicks."
    "\vAdditional rates and delays can be specified, e.g. \"-r 900 -r 1100\". To make use of "
    "additional rates, a cycle bind must be specified using \"--cycle-rate\".";

constexpr struct argp argp_data{options, Parser, 0, doc, nullptr, nullptr, nullptr};

int main(int argc, char *argv[]) {
    std::vector<int> descriptors;
    struct Settings settings{};

    argp_parse(&argp_data, argc, argv, 0, nullptr, &settings);

    if (!settings.delay.empty()) {
        std::printf("Delay set to %.03lf ms\n", settings.delay.front());
    } else {
        settings.delay.push_back(defaults.delay);
    }
    if (!settings.hold_delay.empty()) {
        std::printf("Hold delay set to %.03lf ms\n", settings.hold_delay.front());
    } else {
        settings.hold_delay.push_back(defaults.hold_delay);
    }
    if (settings.burst_length.empty()) {
        settings.burst_length.push_back(defaults.burst_length);
    }
    if (settings.key_binds.empty()) {
        settings.key_binds.insert(Input::Button::Middle);
    }
    if (settings.burst_delay.empty()) {
        settings.burst_delay.push_back(defaults.burst_delay);
    }

    if (settings.devices.empty()) {
        // TODO: Move to input/event

        // reads all the dev input devices and opens all usable ones
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
            if ((filename.find("event-mouse") == std::string::npos &&
                 filename.find("event-kbd") == std::string::npos) ||
                filename.find("if0") != std::string::npos) {
                continue;
            }
            int fd = openat(dirfd, entry->d_name, O_RDONLY | O_NONBLOCK);
            if (fd == -1) {
                int err = errno;
                error(0, err, "could not open %s", filename.c_str());
                continue;
            }
            device_names.push_back(filename);
            descriptors.push_back(fd);
        }
        printf("Found %ld device%s\n", descriptors.size(), descriptors.size() != 1 ? "s" : "");
        for (const auto &name : device_names) {
            std::printf("- %s\n", name.c_str());
        }
        close(dirfd);
    } else {
        for (auto device : settings.devices) {
            int fd = open(device, O_RDONLY | O_NONBLOCK);
            if (fd == -1) {
                int err = errno;
                std::fprintf(stderr, "error %d while trying to open `%s'\n", err, device);
                continue;
            }
            descriptors.push_back(fd);
        }
    }
    if (descriptors.empty()) {
        std::printf("There are no devices available, stopping...\n");
        return 0;
    }

    for (u_int32_t i = 0; i < settings.delay.size(); i++) {
        double delay = settings.delay[i];
        double rpm = 60.0 * 1000.0 / delay;
        std::printf("Delay %d set to %.03fms [%.00f cpm]\n", i, delay, rpm);
    }

    std::stop_source stop;

    // Start the system
    std::unique_ptr<Driver::Driver> driver = std::make_unique<Driver::Uinput>(stop);
    std::vector<std::unique_ptr<Input::Device>> Inputs;
    for (const auto fd : descriptors) {
        Inputs.push_back(std::make_unique<Input::Event>(fd));
    }

    driver->SetDelay(settings.delay.front() * 1000);
    driver->SetHoldTime(settings.hold_delay.front() * 1000);
    driver->SetBurstLength(settings.burst_length.front());
    driver->SetBurstDelay(settings.burst_delay.front() * 1000);

    int status_file_fd{-1};
    if (settings.status_file != nullptr) {
        status_file_fd = open(settings.status_file, O_WRONLY | O_NONBLOCK | O_CREAT, 0664);
        if (status_file_fd == -1) {
            int err = errno;
            std::fprintf(stderr, "error opening status file %d\n", err);
        } else {
            std::printf("Opened status file %s\n", settings.status_file);
        }
    }

    std::vector<std::thread> threads;
    for (const auto &Input : Inputs) {
        threads.push_back(std::thread(
            [&](std::stop_token s) {
                Worker(*Input.get(), *driver.get(), settings, status_file_fd, s);
            },
            stop.get_token()));
    }

    // Signal handling
    struct HandlerData handler_data{status_file_fd, settings.status_file, stop, *driver.get()};
    handler_input = &handler_data;
    std::signal(SIGINT, Handler);

    for (auto &thread : threads) {
        thread.join();
    }

    return 0;
}