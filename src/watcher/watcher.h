#pragma once

// Spawns a thread that watches for input events
#include <memory>
#include <stop_token>
#include <thread>

namespace Driver {
class Driver;
}

namespace Watcher {

enum class Button {
    Left,
    Right,
    Middle,
    Side,
    Extra,
    Forward,
    Back,
    None,
    Invalid,
};

class Watcher {
  public:
    explicit Watcher(Driver::Driver &driver);
    virtual ~Watcher();

    void Start();

  protected:
    void Join();

  private:
    virtual void ReadInput(Button &button, int &value) = 0;

    void Worker(std::stop_token stoken);

    Driver::Driver &driver;

    std::stop_source stop;
    std::unique_ptr<std::jthread> thread;
};
} // namespace Watcher