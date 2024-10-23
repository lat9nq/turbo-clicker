#include "input/event.h"

#include "input/buttons.h"
#include "input/input.h"
#include <array>
#include <bits/types/struct_timeval.h>
#include <cerrno>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <poll.h>
#include <sys/poll.h>
#include <unistd.h>

namespace Input {

using ButtonMapT = std::array<Input::Button, 1 << 12>;

constexpr ButtonMapT Map() {
    ButtonMapT result;
    for (u_int32_t i = 0; i < result.size(); i++) {
        result[i] = Button::Invalid;
    }

    result[KEY_ESC] = Button::Esc;
    result[KEY_1] = Button::One;
    result[KEY_2] = Button::Two;
    result[KEY_3] = Button::Three;
    result[KEY_4] = Button::Four;
    result[KEY_5] = Button::Five;
    result[KEY_6] = Button::Six;
    result[KEY_7] = Button::Seven;
    result[KEY_8] = Button::Eight;
    result[KEY_9] = Button::Nine;
    result[KEY_0] = Button::Zero;
    result[KEY_MINUS] = Button::Minus;
    result[KEY_EQUAL] = Button::Equal;
    result[KEY_BACKSPACE] = Button::BackSpace;
    result[KEY_TAB] = Button::Tab;
    result[KEY_Q] = Button::Q;
    result[KEY_W] = Button::W;
    result[KEY_E] = Button::E;
    result[KEY_R] = Button::R;
    result[KEY_T] = Button::T;
    result[KEY_Y] = Button::Y;
    result[KEY_U] = Button::U;
    result[KEY_I] = Button::I;
    result[KEY_O] = Button::O;
    result[KEY_P] = Button::P;
    result[KEY_LEFTBRACE] = Button::LeftBrace;
    result[KEY_RIGHTBRACE] = Button::RightBracket;
    result[KEY_ENTER] = Button::Enter;
    result[KEY_LEFTCTRL] = Button::LeftControl;
    result[KEY_A] = Button::A;
    result[KEY_S] = Button::S;
    result[KEY_D] = Button::D;
    result[KEY_F] = Button::F;
    result[KEY_G] = Button::G;
    result[KEY_H] = Button::H;
    result[KEY_J] = Button::J;
    result[KEY_K] = Button::K;
    result[KEY_L] = Button::L;
    result[KEY_SEMICOLON] = Button::SemiColon;
    result[KEY_APOSTROPHE] = Button::Apostrophe;
    result[KEY_GRAVE] = Button::Tilde;
    result[KEY_LEFTSHIFT] = Button::LeftShift;
    result[KEY_BACKSLASH] = Button::Backslash;
    result[KEY_Z] = Button::Z;
    result[KEY_X] = Button::X;
    result[KEY_C] = Button::C;
    result[KEY_V] = Button::V;
    result[KEY_B] = Button::B;
    result[KEY_N] = Button::N;
    result[KEY_M] = Button::M;
    result[KEY_COMMA] = Button::Comma;
    result[KEY_SLASH] = Button::Slash;
    result[KEY_RIGHTSHIFT] = Button::RightShift;
    // result[KEY_KPASTERISK] = Button::;
    result[KEY_LEFTALT] = Button::LeftAlt;
    result[KEY_SPACE] = Button::Space;
    result[KEY_CAPSLOCK] = Button::CapsLock;
    result[KEY_F1] = Button::F1;
    result[KEY_F2] = Button::F2;
    result[KEY_F3] = Button::F3;
    result[KEY_F4] = Button::F4;
    result[KEY_F5] = Button::F5;
    result[KEY_F6] = Button::F6;
    result[KEY_F7] = Button::F7;
    result[KEY_F8] = Button::F8;
    result[KEY_F9] = Button::F9;
    result[KEY_F10] = Button::F10;
    result[KEY_F11] = Button::F11;
    result[KEY_F12] = Button::F12;
    result[KEY_F12] = Button::F12;
    result[KEY_KPENTER] = Button::Enter;
    result[KEY_RIGHTCTRL] = Button::RightControl;
    result[KEY_SYSRQ] = Button::PrintScreen;
    result[KEY_RIGHTALT] = Button::RightAlt;
    result[KEY_HOME] = Button::Home;
    result[KEY_UP] = Button::KeyUp;
    result[KEY_PAGEUP] = Button::PageUp;
    result[KEY_LEFT] = Button::KeyLeft;
    result[KEY_RIGHT] = Button::KeyRight;
    result[KEY_END] = Button::End;
    result[KEY_DOWN] = Button::KeyDown;
    result[KEY_PAGEDOWN] = Button::PageDown;
    result[KEY_INSERT] = Button::Insert;
    result[KEY_DELETE] = Button::Delete;
    result[KEY_PAUSE] = Button::Pause;
    result[KEY_COMPOSE] = Button::Compose;

    result[BTN_LEFT] = Button::Left;
    result[BTN_RIGHT] = Button::Right;
    result[BTN_MIDDLE] = Button::Middle;
    result[BTN_SIDE] = Button::Side;
    result[BTN_EXTRA] = Button::Extra;
    result[BTN_FORWARD] = Button::Forward;
    result[BTN_BACK] = Button::Back;

    return result;
}

const static ButtonMapT button_map = Map();

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
    clear_values();
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
        case EV_KEY:
            if (event.value == 2) { // ignore when input value is 2 (keyboard button held)
                clear_values();
                return;
            }
            button = button_map[event.code];
            break;
        case EV_REL: {
            switch (event.code) {
            case REL_WHEEL: {
                if (event.value == 1) {
                    button = Button::ScrollUp;
                } else if (event.value == -1) {
                    button = Button::ScrollDown;
                }
            } break;
            default:
                clear_values();
            }
            break;
        } break;
        default:
            clear_values();
            break;
        }

        value = event.value;
    } while (button == Button::None);
}
} // namespace Input