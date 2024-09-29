#pragma once

#include "driver/driver.h"

namespace Driver {
class Dummy : public Driver {
  public:
    explicit Dummy();
    ~Dummy();

  private:
    void ButtonDown();
    void ButtonUp();
};
} // namespace Driver