#ifndef __EVENT_H__
#define __EVENT_H__

#include <memory>
#include <glm/ext.hpp>

namespace Dental {
  class Event {
  public:
    enum Type {
      RENDER  = 0x0000,
      POINTER = 0x00FF,

      POINTER_PRESS   = 0x0001,
      POINTER_MOVE    = 0x0002,
      POINTER_RELEASE = 0x0003,
      POINTER_SCROLL_UP    = 0x0004,
      POINTER_SCROLL_DOWN  = 0x0005,
      POINTER_SCROLL_LEFT  = 0x0006,
      POINTER_SCROLL_RIGHT = 0x0007,
      POINTER_DOUBLE_CLICK = 0x0008,

      MULTI_POINTER_PRESS   = 0x0010,
      MULTI_POINTER_MOVE    = 0x0020,
      MULTI_POINTER_RELEASE = 0x0040,
    };

    enum Button {
      NO_BUTTON     = 0x00,
      LEFT_BUTTON   = 0x01,
      RIGHT_BUTTON  = 0x02,
      MIDDLE_BUTTON = 0x04
    };

    Event();

    Event(Type type);

    Event(Type type, float x, float y);

    Event(Type type, Button button, float x, float y);

    Event(Type type, float x, float y, float second_x, float second_y);

    ~Event() {}

    void handled(bool handled) { handled_ = handled; }
    float handled() { return handled_; }

    void type(Type type) { type_ = type; }
    const Type& type() const { return type_; }

    void button(Button button) { button_ = button; }
    const Button& button() const { return button_; }

    double referenceTime() const { return time_s_; }

    void firstPoint(float x, float y) {
      first_point_.x = x;
      first_point_.y = y;
    }
    const glm::vec2& firstPoint() const { return first_point_; }

    void secondPoint(float x, float y) {
      second_point_.x = x;
      second_point_.y = y;
    }
    const glm::vec2& secondPoint() const { return second_point_; }

    void firstProjectPoint(float x, float y) {
      first_project_point_.x = x;
      first_project_point_.y = y;
    }
    glm::vec2& firstProjectPoint() { return first_project_point_; }

    void secondProjectPoint(float x, float y) {
      second_project_point_.x = x;
      second_project_point_.y = y;
    }
    glm::vec2& secondProjectPoint() { return second_project_point_; }

  private:
    glm::vec2 first_point_, second_point_;
    glm::vec2 first_project_point_, second_project_point_;

    bool handled_;
    Type type_;
    Button button_;

    double time_s_;
  };

  using EventPtr = std::shared_ptr<Event>;
}
#endif
