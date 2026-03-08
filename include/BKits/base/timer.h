#pragma once

#include <chrono>
#include <functional>
#include <queue>

namespace Bkits {
namespace Base {
using Clock = std::chrono::steady_clock;
using TimeStamp = std::chrono::time_point<Clock>;

struct TimerNode {
  uint64_t id;
  uint64_t ms;
  TimeStamp expires;
  std::function<void()> cb;
  uint64_t repeatCount;

  bool operator>(const TimerNode &other) const {
    if (expires == other.expires) {
      return id > other.id;
    }
    return expires > other.expires;
  }
};

class TimerManager {
public:
  void AddTimer(uint64_t ms, std::function<void()> cb, uint64_t repeat = 0) {
    TimeStamp now = Clock::now();
    TimeStamp expireTime = now + std::chrono::milliseconds(ms);

    heap_.push({GetNextId(), ms, expireTime, std::move(cb), repeat});
  }

  void tick() {
    TimeStamp now = Clock::now();
    while (!heap_.empty() && now >= heap_.top().expires) {
      TimerNode top = heap_.top();
      heap_.pop();

      if (top.cb)
        top.cb();

      if (top.repeatCount > 1) {
        top.expires += std::chrono::milliseconds(top.ms);
        top.repeatCount--;
        heap_.push(top);
      }
    }
  }

  int64_t GetNextTimeOut() {
    if (heap_.empty())
      return -1;
    TimeStamp now = Clock::now();
    if (heap_.top().expires <= now)
      return 0;
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               heap_.top().expires - now)
        .count();
  }
  bool hasActiveTasks() const {
    return !heap_.empty(); // 如果堆空了，说明没定时器了
  }

private:
  uint64_t GetNextId() { return id_++; }
  uint64_t id_;

  std::priority_queue<TimerNode, std::vector<TimerNode>,
                      std::greater<TimerNode>>
      heap_;
};

} // namespace Base

} // namespace Bkits
