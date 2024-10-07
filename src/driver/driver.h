#pragma once

#include <atomic>
#include <limits>
#include <list>
#include <memory>
#include <queue>
#include <stop_token>
#include <sys/types.h>
#include <thread>
#include <utility>

namespace Driver {
class Driver {
  public:
    explicit Driver(const std::stop_source &stop);
    virtual ~Driver();

    void Update(int value);

    void SetDelay(u_int64_t usec);
    void SetHoldTime(u_int64_t usec);
    void SetBurstLength(u_int32_t length);

  private:
    void Activate();
    void PopWorker();
    void Worker(std::stop_token stoken);
    void CleanUp(std::stop_token stoken);

    std::thread Click(std::stop_token &stoken);

    void ButtonDown();
    void ButtonUp();
    virtual void EmitDown() = 0;
    virtual void EmitUp() = 0;

    std::mutex active_mutex;
    std::mutex update_mutex;
    std::mutex transfer_mutex;

    std::queue<std::stop_source> stop_sources;
    std::queue<std::thread> workers;

    std::queue<std::pair<std::stop_source, std::thread>> garbage;
    std::thread garbage_collector;

    const std::stop_source &main_stop;
    std::unique_ptr<std::jthread> thread;

    std::atomic<u_int64_t> delay{66666};
    std::atomic<u_int64_t> hold_time{20000};

    std::atomic<u_int32_t> burst_length{std::numeric_limits<u_int32_t>::max()};

    bool down{};
    std::mutex button_mutex;
};
} // namespace Driver