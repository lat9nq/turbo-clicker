#pragma once

#include "watcher/watcher.h"

namespace Watcher {
class EventWatcher : public Watcher {
  public:
    EventWatcher(const char *device);
    ~EventWatcher();

    bool Valid() const;

  private:
    virtual void ReadInput(Button &button, int &value);

    const char *device_name;
    std::atomic<int> fd{-1};
};
} // namespace Watcher