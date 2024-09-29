#include "watcher/event.h"
#include <memory>

int main() {
    std::unique_ptr<Watcher::Watcher> watcher = std::make_unique<Watcher::EventWatcher>(
        "/dev/input/by-id/usb-Logitech_USB_Receiver-event-mouse");
    watcher->Start();

    return 0;
}