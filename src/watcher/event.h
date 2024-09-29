#pragma once

#include "watcher/watcher.h"
#include <atomic>

namespace Driver {
class Driver;
}

namespace Watcher {
class EventWatcher : public Watcher {
  public:
    explicit EventWatcher(Driver::Driver &driver, const char *device);
    ~EventWatcher();

    bool Valid() const;

  private:
    virtual void ReadInput(Button &button, int &value);

    const char *device_name;
    std::atomic<int> fd{-1};
};
} // namespace Watcher