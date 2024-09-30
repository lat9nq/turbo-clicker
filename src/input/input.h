#pragma once

#include <sys/types.h>

namespace Input {
enum class Button : u_int32_t;
class Device {
  public:
    explicit Device();
    virtual ~Device();

    virtual void ReadInput(Button &button, int &value) = 0;
};
} // namespace Input