#pragma once

#include "input/input.h"
#include <atomic>

namespace Driver {
class Driver;
}

namespace Input {
class Event : public Device {
  public:
    explicit Event(int fd);
    ~Event();

    bool Valid() const;

    virtual void ReadInput(Button &button, int &value);

  private:
    std::atomic<int> fd{-1};
};
} // namespace Input