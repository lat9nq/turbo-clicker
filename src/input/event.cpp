#include "input/event.h"

#include "input/buttons.h"
#include "input/input.h"
#include <bits/types/struct_timeval.h>
#include <cerrno>
#include <linux/input.h>
#include <poll.h>
#include <sys/poll.h>
#include <unistd.h>

namespace Input {
Event::Event(int fd_) : Device(), fd{fd_} {
}

Event::~Event() {
    close(fd);
}

bool Event::Valid() const {
    return fd != -1;
}

void Event::ReadInput(Button &button, int &value) {
    auto clear_values = [&]() {
        button = Button::None;
        value = 0;
    };

    if (fd == -1) {
        clear_values();
        return;
    }

    struct pollfd fds {
        fd, POLLIN, 0
    };
    struct input_event event;
    do {
        int ready = poll(&fds, 1, 500);
        if (ready == 0) {
            clear_values();
            return;
        } else if (ready == -1) {
            int err = errno;
            std::fprintf(stderr, "error waiting for data %d\n", err);
            clear_values();
            button = Button::Invalid;
            return;
        }

        ssize_t size = read(fd, &event, sizeof(event));
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
} // namespace Input