#pragma once

#include <atomic>
#include <memory>
#include <stop_token>
#include <sys/types.h>
#include <thread>

namespace Driver {
class Driver {
  public:
    explicit Driver();
    virtual ~Driver();

    void Update(int value);

    void SetDelay(u_int64_t usec);
    void SetHoldTime(u_int64_t usec);

    void Start();

  protected:
    void Join();

  private:
    void Worker(std::stop_token stoken);

    virtual void ButtonDown() = 0;
    virtual void ButtonUp() = 0;

    std::mutex mutex;

    std::stop_source stop;
    std::unique_ptr<std::jthread> thread;

    std::atomic<u_int64_t> delay{66666};
    std::atomic<u_int64_t> hold_time{20000};
};
} // namespace Driver