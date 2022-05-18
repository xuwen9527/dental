#include <events.h>

namespace Dental {
  Events::Events() {
  }

  void Events::pop(Event& event) {
    std::unique_lock<std::mutex> lock(mutex_);
    //cond_.wait(lock, [this]() { return !events_.empty(); });
    event = std::move(events_.front());
    events_.pop();
  }
  
  void Events::push(Event& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    events_.emplace(event);
    cond_.notify_one();
  }

  void Events::redraw() {
    std::lock_guard<std::mutex> lock(mutex_);
    events_.emplace(Event());
    cond_.notify_one();
  }

  void Events::pause() {
    std::lock_guard<std::mutex> lock(mutex_);
    decltype(events_)().swap(events_);
    cond_.notify_one();
  }

  void Events::swap(Events& events) {
    decltype(events_)().swap(events_);
    cond_.notify_one();
  }

  bool Events::empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return events_.empty();
  }
}