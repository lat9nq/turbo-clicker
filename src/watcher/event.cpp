#include "watcher/event.h"

#include "driver/driver.h"
#include "watcher/watcher.h"
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

namespace Watcher {
EventWatcher::EventWatcher(Driver::Driver &driver, const char *device)
    : Watcher(driver), device_name{device} {
    fd = open(device_name, O_RDONLY);
    if (fd == -1) {
        int err = errno;
        std::fprintf(stderr, "error %d\n", err);
        return;
    }
}

EventWatcher::~EventWatcher() {
    Join();
    // printf("closing\n");
    close(fd);
}

bool EventWatcher::Valid() const {
    return fd != -1;
}

void EventWatcher::ReadInput(Button &button, int &value) {
    auto clear_values = [&]() {
        button = Button::Invalid;
        value = 0;
    };

    if (fd == -1) {
        clear_values();
        return;
    }

    struct input_event event;
    do {
        ssize_t size = read(fd, &event, sizeof(event));
        // printf("read %ld\n", size);
        if (size == -1) {
            clear_values();
            return;
        }

        switch (event.type) {
        case EV_KEY: {
            switch (event.code) {
            case BTN_LEFT:
                button = Button::Left;
                break;
            case BTN_RIGHT:
                button = Button::Right;
                break;
            case BTN_MIDDLE:
                button = Button::Middle;
                break;
            case BTN_SIDE:
                button = Button::Side;
                break;
            case BTN_EXTRA:
                button = Button::Extra;
                break;
            case BTN_FORWARD:
                button = Button::Forward;
                break;
            case BTN_BACK:
                button = Button::Back;
                break;
            default:
                break;
            }
        } break;
        default:
            clear_values();
            break;
        }

        value = event.value;
    } while (event.type != EV_KEY);
}
} // namespace Watcher