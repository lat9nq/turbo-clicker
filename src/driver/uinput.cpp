#include "driver/uinput.h"

#include "driver/driver.h"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <stop_token>
#include <sys/ioctl.h>

namespace Driver {
Uinput::Uinput(const std::stop_source &stop) : Driver(stop) {
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        int err = errno;
        std::fprintf(stderr, "error %d\n", err);
        return;
    }

    // enable mouse button left events
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);

    std::memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "turbo-clicker device");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
}
Uinput::~Uinput() {
    Join();
    close(fd);
}

void Uinput::Emit(int type, int code, int value) {
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = value;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    write(fd, &ie, sizeof(ie));
}

void Uinput::ButtonDown() {
    Emit(EV_KEY, BTN_LEFT, 1);
    Emit(EV_SYN, SYN_REPORT, 0);
}

void Uinput::ButtonUp() {
    Emit(EV_KEY, BTN_LEFT, 0);
    Emit(EV_SYN, SYN_REPORT, 0);
}

} // namespace Driver