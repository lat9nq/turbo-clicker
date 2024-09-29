#include "driver/driver.h"
#include "driver/uinput.h"
#include "watcher/event.h"
#include <memory>

int main() {
    std::unique_ptr<Driver::Driver> driver = std::make_unique<Driver::Uinput>();
    driver->Start();

    std::unique_ptr<Watcher::Watcher> watcher = std::make_unique<Watcher::EventWatcher>(
        *driver.get(), "/dev/input/by-id/usb-Logitech_USB_Receiver-event-mouse");
    watcher->Start();

    return 0;
}