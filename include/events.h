#ifndef __EVENTS_H__
#define __EVENTS_H__

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <event.h>

namespace Dental {
  class Events {
  public:
    Events();

    void pop(Event& event);

    void push(Event& event);

    void redraw();

    void pause();

    void clear();

    void swap(Events& events);

    bool empty();

  private:
    std::queue<Event> events_;

    std::mutex mutex_;
    std::condition_variable cond_;
  };

  using EventsPtr = std::shared_ptr<Events>;
}
#endif
