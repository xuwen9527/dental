// #ifndef __MANIPULATOR_H__
// #define __MANIPULATOR_H__

// #include <memory>
// #include <vector>
// #include "glm/ext.hpp"
// #include "View.h"
// #include "camera.h"
// #include "event.h"
// #include "boundingSphere.h"
// #include "render_info.h"

// namespace Dental {
//   class Manipulator {
//   public:
//     class Viewpoint {
//     public:
//       Viewpoint();

//       Viewpoint(const Viewpoint& viewpoint);

//       Viewpoint(glm::vec3& focal, float range, glm::vec3& eulerAngle);

//       Viewpoint(glm::vec3& focal, float range, glm::quat& quat);

//       ~Viewpoint();

//       Viewpoint operator - (Viewpoint& other);
//       Viewpoint operator + (Viewpoint& other);

//       bool valid();

//       void slerp(float t, Viewpoint& to);

//       void setName(std::string& name) { _name = name; }
//       std::string& getName() { return _name; }

//       void setFocalPoint(glm::vec3& point) { _focalPoint = point; }
//       glm::vec3& getFocalPoint() { return _focalPoint; }

//       void setEulerAngle(glm::vec3& eulerAngle);
//       glm::vec3 getEulerAngle();

//       glm::quat& getQuat() { return _quat; }
//       void setQuat(const glm::quat& quat) { _quat = quat; }

//       void setRange(float range) { _range = range; }
//       float getRange() { return _range; }

//     protected:
//       std::string _name;
//       glm::vec3 _focalPoint;
//       glm::quat _quat;
//       float _range;
//     };

//     using Viewpoints = std::vector<Viewpoint>;

//     Manipulator();
//     ~Manipulator();

//     void setCamera(CameraPtr& camera);

//     bool valid() { return _distance > 0.f; }

//     void setCenter(glm::vec3 &center);
//     const glm::vec3 &getCenter();

//     void setOffset(glm::vec3 &offset);
//     const glm::vec3& getOffset();

//     void setDistance(float distance);
//     float getDistance();

//     void setRotation(glm::quat &rotation);
//     glm::quat &getRotation();

//     void setRotateSpeed(float speed);
//     float getRotateSpeed();

//     void zoom(float scale);
//     // 绕axis旋转angle弧度, mv空间
//     void rotate(glm::vec3& axis, float angle);
//     //四元数旋转, mv空间
//     void rotate(glm::quat& quat);
//     // x,y,z三轴平移量, mv空间
//     void pan(float dx, float dy, float dz);

//     void apply(RenderInfoPtr &renderInfo);

//     void handleEvent(Event& event, View &view);

//     Viewpoint createViewpoint(BoundingSphere& sphere);
//     Viewpoint getViewpoint();
//     void setViewpoint(Viewpoint& viewpoint, float duration_s = 0.0);

//   private:
//     glm::mat4 getMatrix();
//     glm::mat4 getInveseMatrix();

//     bool mouseMove(Event& event, View &view);
//     bool mousePress(Event& event, View &view);
//     bool mouseRelease(Event& event, View &view);
//     bool mouseScroll(Event& event, View &view);
//     bool mouseMultiMove(Event &event, View &view);
//     bool mouseMultiPress(Event& event, View &view);
//     bool mouseMultiRelease(Event& event, View &view);

//     bool rotateTrackball(const glm::vec2& p0, const glm::vec2& p1);

//     void fly(double time_s);

//     glm::vec2 _last_point0;
//     glm::vec2 _last_point1;
//     bool _pointer_pressed;

//     float _distance;
//     glm::vec3 _center;
//     glm::vec3 _offset;
//     glm::quat _rotation;

//     float _trackballSize;
//     float _rotate_speed;

//     float _wheelZoomFactor;
//     float _minimumDistance;
//     float _maximumDistance;

//     //是否绕center旋转
//     bool _rotate_center;

//     CameraWeakPtr _camera;

//     class FlightParams {
//     public:
//       Viewpoint _start_viewpoint;
//       Viewpoint _end_viewpoint;
//       glm::vec3 _start_offset;
//       float _duration_s;
//       double _time_s;

//       bool valid();
//       void reset();
//     };

//     FlightParams _flightParams;
//   };

//   using ManipulatorPtr = std::shared_ptr<Manipulator>;
// }
// #endif
