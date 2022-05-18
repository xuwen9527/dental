#include <event.h>
#include <timer.h>

namespace Dental {
  Event::Event() {
    type_ = RENDER;
    button_ = NO_BUTTON;
    handled_ = false;
    time_s_ = Timer::instance().time_s();
  }

  Event::Event(Type type) {
    type_ = type;
    button_ = NO_BUTTON;
    handled_ = false;
    time_s_ = Timer::instance().time_s();
  }

  Event::Event(Type type, float x, float y) {
    type_ = type;
    button_ = NO_BUTTON;
    handled_ = false;
    time_s_ = Timer::instance().time_s();

    first_point_.x = x;
    first_point_.y = y;
  }

  Event::Event(Type type, Button button, float x, float y) {
    type_ = type;
    button_ = button;
    handled_ = false;
    time_s_ = Timer::instance().time_s();

    first_point_.x = x;
    first_point_.y = y;
  }

  Event::Event(Type type, float x, float y, float second_x, float second_y) {
    type_ = type;
    button_ = NO_BUTTON;
    handled_ = false;
    time_s_ = Timer::instance().time_s();

    first_point_.x = x;
    first_point_.y = y;

    second_point_.x = second_x;
    second_point_.y = second_y;
  }
}