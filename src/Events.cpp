#include <events.h>

namespace Dental {
  Events::Events() {
  }

  void Events::pop(Event& event) {
    event = std::move(events_.front());
    events_.pop();
  }
  
  void Events::push(Event& event) {
    events_.emplace(event);
    cond_.notify_one();
  }

  void Events::redraw() {
    events_.emplace(Event());
    cond_.notify_one();
  }

  void Events::pause() {
    decltype(events_)().swap(events_);
    cond_.notify_one();
  }

  void Events::swap(Events& events) {
    decltype(events_)().swap(events_);
    cond_.notify_one();
  }

  bool Events::empty() {
    return events_.empty();
  }
}