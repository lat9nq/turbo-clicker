#pragma once

#include "watcher/watcher.h"
#include <atomic>

namespace Driver {
class Driver;
}

namespace Watcher {
class EventWatcher : public Watcher {
  public:
    explicit EventWatcher(const char *device);
    ~EventWatcher();

    bool Valid() const;

    virtual void ReadInput(Button &button, int &value);

  private:
    const char *device_name;
    std::atomic<int> fd{-1};
};
} // namespace Watcher