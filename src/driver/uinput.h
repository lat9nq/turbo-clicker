#pragma once

// implementation based on the exampled provided at
// https://www.kernel.org/doc/html/latest/input/uinput.html

#include "driver/driver.h"
#include <linux/uinput.h>

namespace Driver {
class Uinput : public Driver {
  public:
    explicit Uinput();
    ~Uinput();

  private:
    void ButtonDown();
    void ButtonUp();

    void Emit(int type, int code, int value);

    struct uinput_setup usetup;
    int fd;
};
} // namespace Driver