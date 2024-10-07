#pragma once

#include "driver/driver.h"

namespace Driver {
class Dummy : public Driver {
  public:
    explicit Dummy(const std::stop_source &stop);
    ~Dummy();

  private:
    void EmitDown();
    void EmitUp();
};
} // namespace Driver