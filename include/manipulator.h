#ifndef __MANIPULATOR_H__
#define __MANIPULATOR_H__

#include <memory>
#include <vector>
#include <glm/ext.hpp>
#include <camera.h>
#include <event.h>
#include <bounding_sphere.h>
#include <render_info.h>

namespace Dental {
  class Manipulator {
  public:
    class Viewpoint {
    public:
      Viewpoint();
      Viewpoint(const Viewpoint& viewpoint);
      Viewpoint(glm::vec3& focal, float range, glm::vec3& eulerAngle);
      Viewpoint(glm::vec3& focal, float range, glm::quat& quat);

      ~Viewpoint();

      Viewpoint operator - (Viewpoint& other);
      Viewpoint operator + (Viewpoint& other);

      bool valid();

      void slerp(float t, Viewpoint& to);

      void name(std::string& name) { name_ = name; }
      const std::string& name() const { return name_; }

      void focalPoint(glm::vec3& point) { focalPoint_ = point; }
      const glm::vec3& focalPoint() const { return focalPoint_; }

      void eulerAngle(glm::vec3& eulerAngle);
      glm::vec3 eulerAngle() const;

      const glm::quat& quat() const { return quat_; }
      const void quat(const glm::quat& quat) { quat_ = quat; }

      void range(float range) { range_ = range; }
      float range() const { return range_; }

    protected:
      std::string name_;
      glm::vec3 focalPoint_;
      glm::quat quat_;
      float range_;
    };

    using Viewpoints = std::vector<Viewpoint>;

    Manipulator();
    ~Manipulator();

    void camera(const CameraPtr& camera);

    bool valid() { return distance_ > 0.f; }

    void center(const glm::vec3 &center);
    const glm::vec3 &center() const;

    void offset(const glm::vec3 &offset);
    const glm::vec3& offset();

    void distance(float distance);
    float distance();

    void rotation(const glm::quat &rotation);
    glm::quat &rotation();

    void rotateSpeed(float speed);
    float rotateSpeed();

    void zoom(float scale);
    // 绕axis旋转angle弧度, mv空间
    void rotate(glm::vec3& axis, float angle);
    //四元数旋转, mv空间
    void rotate(glm::quat& quat);
    // x,y,z三轴平移量, mv空间
    void pan(float dx, float dy, float dz);

    void apply(RenderInfoPtr &renderInfo);

    bool handleEvent(Event& event);

    Viewpoint createViewpoint(BoundingSphere& sphere);
    Viewpoint viewpoint();
    void viewpoint(Viewpoint& viewpoint, float duration_s = 0.0);

  private:
    glm::mat4 matrix();
    glm::mat4 inveseMatrix();

    bool mouseMove(Event& event);
    bool mousePress(Event& event);
    bool mouseRelease(Event& event);
    bool mouseScroll(Event& event);
    bool mouseMultiMove(Event &event);
    bool mouseMultiPress(Event& event);
    bool mouseMultiRelease(Event& event);

    bool rotateTrackball(const glm::vec2& p0, const glm::vec2& p1);

    void fly(float time_s);

    glm::vec2 last_point0_;
    glm::vec2 last_point1_;
    bool pointer_pressed_;

    float distance_;
    glm::vec3 center_;
    glm::vec3 offset_; 
    glm::quat rotation_;

    float trackballSize_;
    float rotate_speed_;

    float wheelZoomFactor_;
    float minimumDistance_;
    float maximumDistance_;

    //是否绕center旋转
    bool rotate_center_;

    CameraWeakPtr camera_;

    class FlightParams {
    public:
      Viewpoint start_viewpoint_;
      Viewpoint end_viewpoint_;
      glm::vec3 start_offset_;
      float duration_s_;
      double time_s_;

      bool valid();
      void reset();
    };

    FlightParams flightParams_;
  };

  using ManipulatorPtr = std::shared_ptr<Manipulator>;
}
#endif
