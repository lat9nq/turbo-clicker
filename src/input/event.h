#pragma once

#include "input/input.h"
#include <atomic>

namespace Driver {
class Driver;
}

namespace Watcher {
class EventWatcher : public Watcher {
  public:
    explicit EventWatcher(int fd);
    ~EventWatcher();

    bool Valid() const;

    virtual void ReadInput(Button &button, int &value);

  private:
    std::atomic<int> fd{-1};
};
} // namespace Watcher