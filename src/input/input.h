#pragma once

#include <sys/types.h>

namespace Watcher {
enum class Button : u_int32_t;
class Watcher {
  public:
    explicit Watcher();
    virtual ~Watcher();

    virtual void ReadInput(Button &button, int &value) = 0;
};
} // namespace Watcher