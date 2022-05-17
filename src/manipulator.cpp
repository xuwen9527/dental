#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "manipulator.h"
#include "timer.h"

namespace {
  /**
   * Helper trackball method that projects an x,y pair onto a sphere of radius r OR
   * a hyperbolic sheet if we are away from the center of the sphere.
   */
  double tb_project_to_sphere(double r, double x, double y) {
    double d, t, z;

    d = sqrt(x * x + y * y);
    /* Inside sphere */
    if (d < r * 0.70710678118654752440) {
        z = sqrt(r * r - d * d);
    }                            /* On hyperbola */
    else {
        t = r / 1.41421356237309504880;
        z = t * t / d;
    }
    return z;
  }

  void trackball(glm::quat &rotate, glm::vec3 &axis, float &angle, float trackballSize, float p1x,
                 float p1y, float p2x, float p2y) {
     /*
      * First, figure out z-coordinates for projection of P1 and P2 to
      * deformed sphere
      */
      // glm::vec3 uv(glm::vec3(0.0f, 1.0f, 0.0f) * rotate);
      // glm::vec3 sv(glm::vec3(1.0f, 0.0f, 0.0f) * rotate);
      // glm::vec3 lv(glm::vec3(0.0f, 0.0f, -1.0f) * rotate);

      // glm::vec3 p1 = sv * p1x + uv * p1y - lv * tb_project_to_sphere(trackballSize, p1x, p1y);
      // glm::vec3 p2 = sv * p2x + uv * p2y - lv * tb_project_to_sphere(trackballSize, p2x, p2y);

      glm::dvec3 p1(p1x, p1y, tb_project_to_sphere(trackballSize, p1x, p1y));
      glm::dvec3 p2(p2x, p2y, tb_project_to_sphere(trackballSize, p2x, p2y));

      /*
       * Now, we want the cross product of P1 and P2
       */
      axis = glm::cross(p2, p1);
      axis = glm::normalize(axis);
      /*
        * Figure out how much to rotate around that axis.
        */
      glm::dvec3 e = p2 - p1;
      double d = sqrt(e.x * e.x + e.y * e.y + e.z * e.z);
      double t = d / (2.0f * trackballSize);

      /*
        * Avoid problems with out-of-control values...
        */
      if (t > 1.0) t = 1.0;
      if (t < -1.0) t = -1.0;
      angle = asin(t);
  }

  // a reasonable approximation of cosine interpolation
  double smoothStepInterp(double t) {
    return (t * t) * (3.0 - 2.0 * t);
  }

  // rough approximation of pow(x,y)
  double powFast(double x, double y) {
    return x / (x + y - y * x);
  }

  // accel/decel curve (a < 0 => decel)
  double accelerationInterp(double t, double a) {
    return a == 0.0 ? t : a > 0.0 ? powFast(t, a) : 1.0 - powFast(1.0 - t, -a);
  }
}

namespace Dental {
  Manipulator::Viewpoint::Viewpoint() : range_(-1.0f),
    focalPoint_(0.f, 0.f, 0.f),
    quat_(glm::identity<glm::quat>()) {
  }

  Manipulator::Viewpoint::Viewpoint(const Manipulator::Viewpoint &viewpoint) {
    name_ = viewpoint.name_;
    quat_ = viewpoint.quat_;
    range_ = viewpoint.range_;
    focalPoint_ = viewpoint.focalPoint_;
  }

  Manipulator::Viewpoint::~Viewpoint() {
  }

  Manipulator::Viewpoint::Viewpoint(glm::vec3 &focal, float range, glm::vec3 &angle)
    : focalPoint_(focal), range_(range) {
    eulerAngle(angle);
  }

  Manipulator::Viewpoint::Viewpoint(glm::vec3 &focal, float range, glm::quat &quat)
    : focalPoint_(focal), quat_(quat), range_(range) {
  }

  bool Manipulator::Viewpoint::valid() {
    return range_ > 0.f;
  }

  Manipulator::Viewpoint Manipulator::Viewpoint::operator-(Manipulator::Viewpoint &other) {
    glm::vec3 focalPoint = focalPoint_ - other.focalPoint_;
    double range = range_ - other.range_;
    glm::quat quat = quat_ * glm::inverse(other.quat_);

    return Viewpoint(focalPoint, range, quat);
  }

  Manipulator::Viewpoint Manipulator::Viewpoint::operator+(Manipulator::Viewpoint &other) {
    glm::vec3 focalPoint = focalPoint_ + other.focalPoint_;
    double range = range_ + other.range_;
    glm::quat quat = quat_ * other.quat_;

    return Viewpoint(focalPoint, range, quat);
  }

  void Manipulator::Viewpoint::slerp(float t, Viewpoint &to) {
    focalPoint_ = focalPoint_ + (to.focalPoint() - focalPoint_) * t;
    range_ = range_ + (to.range() - range_) * t;
    quat_ = glm::slerp(quat_, to.quat(), t);
  }

  void Manipulator::Viewpoint::eulerAngle(glm::vec3 &eulerAngle) {
    quat_ = glm::quat(eulerAngle);
  }

  glm::vec3 Manipulator::Viewpoint::eulerAngle() const {
    return glm::eulerAngles(quat_);
  }

  bool Manipulator::FlightParams::valid() {
    return duration_s_ > 0.000001f;
  }

  void Manipulator::FlightParams::reset() {
    duration_s_ = 0.0f;
  }

  Manipulator::Manipulator() :
    center_(0.f, 0.f, 0.f),
    offset_(0.f, 0.f, 0.f), 
    rotation_(glm::identity<glm::quat>()) {
    distance_ = -1.0f;

    wheelZoomFactor_ = 0.1f;
    minimumDistance_ = 0.001f;
    maximumDistance_ = 100.f;

    trackballSize_ = 0.8f;

    rotate_speed_ = 3.f;

    rotate_center_ = false;

    pointer_pressed_ = false;
  }

  Manipulator::~Manipulator() {

  }

  void Manipulator::camera(const CameraPtr &camera) {
    camera_ = camera;
  }

  void Manipulator::center(const glm::vec3 &center) {
    center_ = center;
  }

  const glm::vec3 &Manipulator::center() const {
    return center_;
  }

  void Manipulator::offset(const glm::vec3 &offset) {
    offset_ = offset;
  }

  const glm::vec3 &Manipulator::offset() {
    return offset_;
  }

  void Manipulator::distance(float distance) {
    distance_ = distance;

    wheelZoomFactor_ = distance_ * 0.0001f;
    minimumDistance_ = distance_ * 0.01f;
    maximumDistance_ = distance_ * 3.f;
  }

  float Manipulator::distance() {
    return distance_;
  }

  void Manipulator::rotation(const glm::quat &rotation) {
    rotation_ = rotation;
  }

  glm::quat &Manipulator::rotation() {
    return rotation_;
  }

  void Manipulator::rotateSpeed(float speed) {
    if (speed > 0.01f) {
      rotate_speed_ = speed;
    }
  }

  float Manipulator::rotateSpeed() {
    return rotate_speed_;
  }

  bool Manipulator::rotateTrackball(const glm::vec2 &p0, const glm::vec2 &p1) {
    if (fabs(p0.x - p1.x) < 0.0000001f && fabs(p0.y - p1.y) < 0.0000001f) {
      return false;
    }

    glm::vec3 axis;
    float angle;
    trackball(rotation_, axis, angle, trackballSize_, p1.x, p1.y, p0.x, p0.y);
    rotation_ = glm::rotate(rotation_, angle * rotate_speed_, axis);

    return true;
  }

  glm::mat4 Manipulator::matrix() {
    return glm::translate(glm::vec3(0.0, 0.0, -distance_)) *
           glm::translate(-offset_) *
           glm::mat4_cast(glm::inverse(rotation_)) *
           glm::translate(-center_);
  }

  glm::mat4 Manipulator::inveseMatrix() {
    return glm::translate(center_) *
           glm::mat4_cast(rotation_) *
           glm::translate(offset_) *
           glm::translate(glm::vec3(0., 0., distance_));
  }

  void Manipulator::rotate(glm::vec3 &axis, float angle) {
    rotation_ = glm::rotate(rotation_, angle, axis);
  }

  void Manipulator::rotate(glm::quat &quat) {
    rotation_ *= quat;
  }

  void Manipulator::pan(float dx, float dy, float dz) {
    if (rotate_center_) {
      center_.x -= dx;
      center_.y -= dy;
      center_.z -= dz;
    } else {
      offset_.x -= dx;
      offset_.y -= dy;
      offset_.z -= dz;
    }
  }

  void Manipulator::zoom(float dt) {
    float scale = 1.0f + dt;

    float newDistance = distance_ * scale;
    if (newDistance > minimumDistance_) {
      if (newDistance < maximumDistance_) {
        distance_ = newDistance;
      } else {
        distance_ = maximumDistance_;
      }
    } else {
      distance_ = minimumDistance_;
    }
  }

  void Manipulator::apply(RenderInfoPtr &renderInfo) {
    //glm::vec3 center = sphere.center();
    //glm::vec3 eye = center + glm::vec3(0.f, 0.f, sphere.radius() * 4.5f);
    //camera.getMV() = glm::lookAt(eye, center, glm::vec3(0.0f, 1.0f, 0.0f));
    if (valid()) {
      CameraPtr camera = camera_.lock();
      if (camera) {
        if (flightParams_.valid()) {
          fly(Timer::instance().time_s());
          // renderInfo->needRedraw();
        }
        camera->mv(matrix());
        renderInfo->mvp(camera->mv(), camera->projection());
      }
    }
  }

  // void Manipulator::handleEvent(Event &event, View &view) {
  //   if (event.getHandled()) {
  //     return;
  //   }

  //   bool handled = false;
  //   switch (event.getType()) {
  //     case Event::POINTER_PRESS:
  //       handled = mousePress(event, view);
  //       break;
  //     case Event::POINTER_MOVE:
  //       flightParams_.reset();
  //       handled = mouseMove(event, view);
  //       break;
  //     case Event::POINTER_RELEASE:
  //       handled = mouseRelease(event, view);
  //       break;
  //     case Event::MULTI_POINTER_PRESS:
  //       handled = mouseMultiPress(event, view);
  //       break;
  //     case Event::MULTI_POINTER_MOVE:
  //       flightParams_.reset();
  //       handled = mouseMultiMove(event, view);
  //       break;
  //     case Event::MULTI_POINTER_RELEASE:
  //       handled = mouseMultiRelease(event, view);
  //       break;
  //     case Event::POINTER_SCROLL_UP:
  //     case Event::POINTER_SCROLL_DOWN:
  //     case Event::POINTER_SCROLL_LEFT:
  //     case Event::POINTER_SCROLL_RIGHT:
  //       handled = mouseScroll(event, view);
  //       break;
  //     default:
  //       break;
  //   }

  //   event.setHandled(handled);
  // }

  // bool Manipulator::mousePress(Event &event, View &view) {
  //   last_point0_ = event.getFirstProjectPoint();
  //   pointer_pressed_ = true;
  //   return false;
  // }

  // bool Manipulator::mouseMove(Event &event, View &view) {
  //   if (!pointer_pressed_) {
  //     return false;
  //   }

  //   if (event.getButton() == Event::LEFT_BUTTON) {
  //     bool flag = rotateTrackball(event.getFirstProjectPoint(), last_point0_);
  //     last_point0_ = event.getFirstProjectPoint();
  //     if (flag) view.needRedraw();
  //     return flag;
  //   }

  //   //平移
  //   if (event.getButton() == Event::MIDDLE_BUTTON) {
  //     CameraPtr safe_camera = camera_.lock();
  //     if (!safe_camera) {
  //       return false;
  //     }

  //     float right, left, top, bottom, near, far;
  //     if (safe_camera->frustum(left, right, bottom, top, near, far)) {
  //       glm::vec2 point0 = last_point0_;
  //       last_point0_ = event.getFirstProjectPoint();

  //       glm::vec2 e = last_point0_ - point0;

  //       //计算到投影空间,逆归一化
  //       float fovy = atanf(top / near) - atanf(bottom / near);
  //       e = e / near * distance_ * tanf(fovy / 2.f);
  //       e.x *= (right - left) * 2.f;
  //       e.y *= (top - bottom) * 2.f;

  //       pan(e.x, e.y, 0.f);

  //       view.needRedraw();
  //       return true;
  //     }
  //   }
  //   return false;
  // }

  // bool Manipulator::mouseRelease(Event &event, View &view) {
  //   last_point0_ = event.getFirstProjectPoint();
  //   pointer_pressed_ = false;
  //   return false;
  // }

  // bool Manipulator::mouseMultiMove(Event &event, View &view) {
  //   if (!pointer_pressed_) {
  //     return false;
  //   }

  //   glm::vec2 point0 = last_point0_;
  //   glm::vec2 point1 = last_point1_;

  //   last_point0_ = event.getFirstProjectPoint();
  //   last_point1_ = event.getSecondProjectPoint();

  //   CameraPtr safe_camera = camera_.lock();
  //   if (!safe_camera) {
  //     return false;
  //   }

  //   glm::vec2 e0 = last_point0_ - point0;
  //   glm::vec2 e1 = last_point1_ - point1;

  //   glm::vec2 we0, we1;
  //   safe_camera->getViewport().length_projectToWindow(e0, we0);
  //   safe_camera->getViewport().length_projectToWindow(e1, we1);

  //   float dist_e0 = glm::length(we0);
  //   float dist_e1 = glm::length(we1);
  //   if ((dist_e0 <= 1.f) && (dist_e1 <= 1.f)) {
  //     return false;
  //   }

  //   float max_dist = dist_e0;
  //   float min_dist = dist_e1;

  //   if (dist_e0 < dist_e1) {
  //     min_dist = dist_e0;
  //     max_dist = dist_e1;
  //   }

  //   bool ret = false;
  //   if ((min_dist < 2.f) && (max_dist > min_dist * 1.5f)) {//旋转
  //     glm::vec2 mid = (last_point0_ + last_point1_) / 2.f;
  //     if (dist_e0 > dist_e1) {
  //       ret = rotateTrackball(last_point0_ - mid, point0 - mid);
  //     } else {
  //       ret = rotateTrackball(last_point1_ - mid, point1 - mid);
  //     }
  //   } else {
  //     float prev_dt = glm::distance(point0, point1);
  //     float last_dt = glm::distance(last_point0_, last_point1_);
  //     float dt = last_dt - prev_dt;
  //     if (fabsf(dt) > 0.01f) {//缩放
  //       zoom(dt > 0.f ? -wheelZoomFactor_ : wheelZoomFactor_);
  //       ret = true;
  //     }

  //     //平移
  //     float right, left, top, bottom, near, far;
  //     if (safe_camera->getFrustum(left, right, bottom, top, near, far)) {
  //       glm::vec2 e = (e0 + e1) / 2.f;
  //       //计算到投影空间,逆归一化
  //       float fovy = atanf(top / near) - atanf(bottom / near);
  //       e = e / near * distance_ * tanf(fovy / 2.f);
  //       e.x *= (right - left) * 2.f;
  //       e.y *= (top - bottom) * 2.f;

  //       pan(e.x, e.y, 0.f);

  //       ret = true;
  //     }
  //   }

  //   if (ret) {
  //     view.needRedraw();
  //   }
  //   return ret;
  // }

  // bool Manipulator::mouseScroll(Event &event, View &view) {
  //   last_point0_ = event.getFirstProjectPoint();
  //   last_point1_ = event.getSecondProjectPoint();
  //   zoom((event.getType() == Event::POINTER_SCROLL_UP || event.getType() == Event::POINTER_SCROLL_LEFT)
  //     ? -wheelZoomFactor_ : wheelZoomFactor_);

  //   view.needRedraw();
  //   return true;
  // }

  // bool Manipulator::mouseMultiPress(Event &event, View &view) {
  //   last_point0_ = event.getFirstProjectPoint();
  //   last_point1_ = event.getSecondProjectPoint();
  //   pointer_pressed_ = true;
  //   return false;
  // }

  // bool Manipulator::mouseMultiRelease(Event &event, View &view) {
  //   last_point0_ = event.getFirstProjectPoint();
  //   last_point1_ = event.getSecondProjectPoint();
  //   pointer_pressed_ = false;
  //   return false;
  // }

  Manipulator::Viewpoint Manipulator::createViewpoint(BoundingSphere &sphere) {
    if (sphere.valid()) {
      CameraPtr camera = camera_.lock();
      if (camera) {
        float dist = sphere.radius();

        float left, right, bottom, top, zNear, zFar;
        camera->splitFrustum(left, right, bottom, top, zNear, zFar);

        double vertical2 = fabs(right - left) / zNear / 2.0;
        double horizontal2 = fabs(top - bottom) / zNear / 2.0;
        double dim = horizontal2 < vertical2 ? horizontal2 : vertical2;
        double viewAngle = atan2(dim, 1.0);
        if (fabs(viewAngle) > 0.00000001f) {
          dist /= sin(viewAngle);
        }

        glm::quat quat(glm::identity<glm::quat>());
        return Viewpoint(sphere.center(), dist, quat);
      }
    }

    return Viewpoint();
  }

  Manipulator::Viewpoint Manipulator::viewpoint() {
    glm::quat rot = glm::inverse(rotation_);
    return Viewpoint(center_, distance_, rot);
  }

  void Manipulator::viewpoint(Viewpoint &vp, float duration_s) {
    if (!vp.valid()) {
      return;
    }

    if (duration_s > 0.000001f) {
      flightParams_.start_viewpoint_ = viewpoint();
      flightParams_.end_viewpoint_ = vp;
      flightParams_.start_offset_ = offset_;
      flightParams_.duration_s_ = duration_s;
      flightParams_.time_s_ = Timer::instance().time_s();
    } else {
      center(vp.focalPoint());
      distance(vp.range());

      glm::vec3 offset(0.f, 0.f, 0.f);
      this->offset(offset);

      glm::quat quat(glm::inverse(vp.quat()));
      rotation(quat);
    }
  }

  void Manipulator::fly(float time_s) {
    if (!flightParams_.valid()) {
      return;
    }

    float t = (time_s - flightParams_.time_s_) / flightParams_.duration_s_;
    float tp = t;

    if (t >= 1.0) {
      flightParams_.duration_s_ = 0.0;
    } else {
      tp = accelerationInterp(tp, 0.4);

      // the more smoothsteps you do, the more pronounced the fade-in/out effect
      tp = smoothStepInterp(tp);
      tp = smoothStepInterp(tp);
    }

    Viewpoint new_vp = flightParams_.start_viewpoint_;
    new_vp.slerp(tp, flightParams_.end_viewpoint_);

    viewpoint(new_vp);

    glm::vec3 offset = flightParams_.start_offset_ * (1.0f - tp);
    offset_ = offset;
  }
}
