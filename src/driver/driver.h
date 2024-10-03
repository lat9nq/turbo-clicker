#pragma once

#include <atomic>
#include <memory>
#include <stop_token>
#include <sys/types.h>
#include <thread>

namespace Driver {
class Driver {
  public:
    explicit Driver(const std::stop_source &stop);
    virtual ~Driver();

    void Update(int value);

    void SetDelay(u_int64_t usec);
    void SetHoldTime(u_int64_t usec);
    void SetBurstLength(u_int32_t length);

    void Start();

  protected:
    void Join();

  private:
    void Worker(std::stop_token stoken);

    void Click();
    std::thread click;

    virtual void ButtonDown() = 0;
    virtual void ButtonUp() = 0;

    std::mutex mutex;
    std::mutex update_mutex;

    const std::stop_source &stop;
    std::unique_ptr<std::jthread> thread;

    std::atomic<u_int64_t> delay{66666};
    std::atomic<u_int64_t> hold_time{20000};

    std::atomic<u_int32_t> burst_length{0};
    std::atomic<u_int32_t> burst_count{0};
};
} // namespace Driver