#include "input/event.h"

#include "input/buttons.h"
#include "input/input.h"
#include <bits/types/struct_timeval.h>
#include <linux/input.h>
#include <sys/select.h>
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
        button = Button::Invalid;
        value = 0;
    };

    if (fd == -1) {
        clear_values();
        return;
    }

    struct input_event event;
    fd_set set;
    FD_SET(fd, &set);
    struct timeval timeout {
        0, 500 * 1000
    };
    do {
        int count = select(1, &set, nullptr, nullptr, &timeout);
        if (count == 0) {
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