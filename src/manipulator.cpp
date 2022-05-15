// #include "manipulator.h"
// #include "timer.h"

// namespace {
//     /**
//      * Helper trackball method that projects an x,y pair onto a sphere of radius r OR
//      * a hyperbolic sheet if we are away from the center of the sphere.
//      */
//     double tb_project_to_sphere(double r, double x, double y) {
//         double d, t, z;

//         d = sqrt(x * x + y * y);
//         /* Inside sphere */
//         if (d < r * 0.70710678118654752440) {
//             z = sqrt(r * r - d * d);
//         }                            /* On hyperbola */
//         else {
//             t = r / 1.41421356237309504880;
//             z = t * t / d;
//         }
//         return z;
//     }

//     void trackball(glm::quat &rotate, glm::vec3 &axis, float &angle, float trackballSize, float p1x,
//                    float p1y, float p2x, float p2y) {
//         /*
//          * First, figure out z-coordinates for projection of P1 and P2 to
//          * deformed sphere
//          */
// //        glm::vec3 uv(glm::vec3(0.0f, 1.0f, 0.0f) * rotate);
// //        glm::vec3 sv(glm::vec3(1.0f, 0.0f, 0.0f) * rotate);
// //        glm::vec3 lv(glm::vec3(0.0f, 0.0f, -1.0f) * rotate);
// //
// //        glm::vec3 p1 = sv * p1x + uv * p1y - lv * tb_project_to_sphere(trackballSize, p1x, p1y);
// //        glm::vec3 p2 = sv * p2x + uv * p2y - lv * tb_project_to_sphere(trackballSize, p2x, p2y);

//         glm::dvec3 p1(p1x, p1y, tb_project_to_sphere(trackballSize, p1x, p1y));
//         glm::dvec3 p2(p2x, p2y, tb_project_to_sphere(trackballSize, p2x, p2y));

//         /*
//          * Now, we want the cross product of P1 and P2
//          */
//         axis = glm::cross(p2, p1);
//         axis = glm::normalize(axis);
//         /*
//          * Figure out how much to rotate around that axis.
//          */
//         glm::dvec3 e = p2 - p1;
//         double d = sqrt(e.x * e.x + e.y * e.y + e.z * e.z);
//         double t = d / (2.0f * trackballSize);

//         /*
//          * Avoid problems with out-of-control values...
//          */
//         if (t > 1.0) t = 1.0;
//         if (t < -1.0) t = -1.0;
//         angle = asin(t);
//     }

//     // a reasonable approximation of cosine interpolation
//     double
//     smoothStepInterp(double t) {
//         return (t * t) * (3.0 - 2.0 * t);
//     }

//     // rough approximation of pow(x,y)
//     double
//     powFast(double x, double y) {
//         return x / (x + y - y * x);
//     }

//     // accel/decel curve (a < 0 => decel)
//     double
//     accelerationInterp(double t, double a) {
//         return a == 0.0 ? t : a > 0.0 ? powFast(t, a) : 1.0 - powFast(1.0 - t, -a);
//     }
// }

// namespace Dental {
//     Manipulator::Viewpoint::Viewpoint() : _range(-1.0f) {
//     }

//     Manipulator::Viewpoint::Viewpoint(const Manipulator::Viewpoint &viewpoint) {
//         _name = viewpoint._name;
//         _quat = viewpoint._quat;
//         _range = viewpoint._range;
//         _focalPoint = viewpoint._focalPoint;
//     }

//     Manipulator::Viewpoint::~Viewpoint() {
//     }

//     Manipulator::Viewpoint::Viewpoint(glm::vec3 &focal, float range, glm::vec3 &eulerAngle)
//             : _focalPoint(focal), _range(range) {
//         setEulerAngle(eulerAngle);
//     }

//     Manipulator::Viewpoint::Viewpoint(glm::vec3 &focal, float range, glm::quat &quat)
//             : _focalPoint(focal), _quat(quat), _range(range) {
//     }

//     bool Manipulator::Viewpoint::valid() {
//         return _range > 0.f;
//     }

//     Manipulator::Viewpoint Manipulator::Viewpoint::operator-(Manipulator::Viewpoint &other) {
//         glm::vec3 focalPoint = _focalPoint - other._focalPoint;
//         double range = _range - other._range;
//         glm::quat quat = _quat * glm::inverse(other._quat);

//         return Viewpoint(focalPoint, range, quat);
//     }

//     Manipulator::Viewpoint Manipulator::Viewpoint::operator+(Manipulator::Viewpoint &other) {
//         glm::vec3 focalPoint = _focalPoint + other._focalPoint;
//         double range = _range + other._range;
//         glm::quat quat = _quat * other._quat;

//         return Viewpoint(focalPoint, range, quat);
//     }

//     void Manipulator::Viewpoint::slerp(float t, Viewpoint &to) {
//         _focalPoint = _focalPoint + (to.getFocalPoint() - _focalPoint) * t;
//         _range = _range + (to.getRange() - _range) * t;
//         _quat = glm::slerp(_quat, to.getQuat(), t);
//     }

//     void Manipulator::Viewpoint::setEulerAngle(glm::vec3 &eulerAngle) {
//         _quat = glm::quat(eulerAngle);
//     }

//     glm::vec3 Manipulator::Viewpoint::getEulerAngle() {
//         return glm::eulerAngles(_quat);
//     }

//     bool Manipulator::FlightParams::valid() {
//         return _duration_s > 0.000001f;
//     }

//     void Manipulator::FlightParams::reset() {
//         _duration_s = 0.0f;
//     }

//     Manipulator::Manipulator() {
//         _center.x = 0.f;
//         _center.y = 0.f;
//         _center.z = 0.f;
//         _distance = -1.0f;

//         _wheelZoomFactor = 0.1f;
//         _minimumDistance = 0.001f;
//         _maximumDistance = 100.f;

//         _trackballSize = 0.8f;

//         _rotate_speed = 3.f;

//         _rotate_center = false;

//         _pointer_pressed = false;
//     }

//     Manipulator::~Manipulator() {

//     }

//     void Manipulator::setCamera(CameraPtr &camera) {
//         _camera = camera;
//     }

//     void Manipulator::setCenter(glm::vec3 &center) {
//         _center = center;
//     }

//     const glm::vec3 &Manipulator::getCenter() {
//         return _center;
//     }

//     void Manipulator::setOffset(glm::vec3 &offset) {
//         _offset = offset;
//     }

//     const glm::vec3 &Manipulator::getOffset() {
//         return _offset;
//     }

//     void Manipulator::setDistance(float distance) {
//         _distance = distance;

//         _wheelZoomFactor = _distance * 0.0001f;
//         _minimumDistance = _distance * 0.01f;
//         _maximumDistance = _distance * 3.f;
//     }

//     float Manipulator::getDistance() {
//         return _distance;
//     }

//     void Manipulator::setRotation(glm::quat &rotation) {
//         _rotation = rotation;
//     }

//     glm::quat &Manipulator::getRotation() {
//         return _rotation;
//     }

//     void Manipulator::setRotateSpeed(float speed) {
//         if (speed > 0.01f)
//             _rotate_speed = speed;
//     }

//     float Manipulator::getRotateSpeed() {
//         return _rotate_speed;
//     }

//     bool Manipulator::rotateTrackball(const glm::vec2 &p0, const glm::vec2 &p1) {
//         if (fabs(p0.x - p1.x) < 0.0000001f && fabs(p0.y - p1.y) < 0.0000001f) {
//             return false;
//         }

//         glm::vec3 axis;
//         float angle;
//         trackball(_rotation, axis, angle, _trackballSize, p1.x, p1.y, p0.x, p0.y);
//         _rotation = glm::rotate(_rotation, angle * _rotate_speed, axis);

//         return true;
//     }

//     glm::mat4 Manipulator::getMatrix() {
//         return glm::translate(glm::vec3(0.0, 0.0, -_distance)) *
//                glm::translate(-_offset) *
//                glm::mat4_cast(glm::inverse(_rotation)) *
//                glm::translate(-_center);
//     }

//     glm::mat4 Manipulator::getInveseMatrix() {
//         return glm::translate(_center) *
//                glm::mat4_cast(_rotation) *
//                glm::translate(_offset) *
//                glm::translate(glm::vec3(0., 0., _distance));
//     }

//     void Manipulator::rotate(glm::vec3 &axis, float angle) {
//         _rotation = glm::rotate(_rotation, angle, axis);
//     }

//     void Manipulator::rotate(glm::quat &quat) {
//         _rotation *= quat;
//     }

//     void Manipulator::pan(float dx, float dy, float dz) {
//         if (_rotate_center) {
//             _center.x -= dx;
//             _center.y -= dy;
//             _center.z -= dz;
//         } else {
//             _offset.x -= dx;
//             _offset.y -= dy;
//             _offset.z -= dz;
//         }
//     }

//     void Manipulator::zoom(float dt) {
//         float scale = 1.0f + dt;

//         float newDistance = _distance * scale;
//         if (newDistance > _minimumDistance) {
//             if (newDistance < _maximumDistance)
//                 _distance = newDistance;
//             else
//                 _distance = _maximumDistance;
//         } else {
//             _distance = _minimumDistance;
//         }
//     }

//     void Manipulator::apply(RenderInfoPtr &renderInfo) {
//         //glm::vec3 center = sphere.center();
//         //glm::vec3 eye = center + glm::vec3(0, 0, sphere.radius() * 4.5f);
//         //camera.getMV() = glm::lookAt(eye, center, glm::vec3(0.0f, 1.0f, 0.0f));
//         if (valid()) {
//             CameraPtr camera = _camera.lock();
//             if (camera) {
//                 if (_flightParams.valid()) {
//                     fly(Timer::instance()->time_s());
//                     renderInfo->needRedraw();
//                 }
//                 camera->setMV(getMatrix());
//                 renderInfo->setMVP(camera->getMV(), camera->getProjection());
//             }
//         }
//     }

//     void Manipulator::handleEvent(Event &event, View &view) {
//         if (event.getHandled())
//             return;

//         bool handled = false;
//         switch (event.getType()) {
//             case Event::POINTER_PRESS:
//                 handled = mousePress(event, view);
//                 break;
//             case Event::POINTER_MOVE:
//                 _flightParams.reset();
//                 handled = mouseMove(event, view);
//                 break;
//             case Event::POINTER_RELEASE:
//                 handled = mouseRelease(event, view);
//                 break;
//             case Event::MULTI_POINTER_PRESS:
//                 handled = mouseMultiPress(event, view);
//                 break;
//             case Event::MULTI_POINTER_MOVE:
//                 _flightParams.reset();
//                 handled = mouseMultiMove(event, view);
//                 break;
//             case Event::MULTI_POINTER_RELEASE:
//                 handled = mouseMultiRelease(event, view);
//                 break;
//             case Event::POINTER_SCROLL_UP:
//             case Event::POINTER_SCROLL_DOWN:
//             case Event::POINTER_SCROLL_LEFT:
//             case Event::POINTER_SCROLL_RIGHT:
//                 handled = mouseScroll(event, view);
//                 break;
//             default:
//                 break;
//         }

//         event.setHandled(handled);
//     }

//     bool Manipulator::mousePress(Event &event, View &view) {
//         _last_point0 = event.getFirstProjectPoint();
//         _pointer_pressed = true;
//         return false;
//     }

//     bool Manipulator::mouseMove(Event &event, View &view) {
//         if (!_pointer_pressed)
//             return false;

//         if (event.getButton() == Event::LEFT_BUTTON) {
//             bool flag = rotateTrackball(event.getFirstProjectPoint(), _last_point0);
//             _last_point0 = event.getFirstProjectPoint();
//             if (flag) view.needRedraw();
//             return flag;
//         }

//         //平移
//         if (event.getButton() == Event::MIDDLE_BUTTON) {
//             CameraPtr safe_camera = _camera.lock();
//             if (!safe_camera) return false;

//             float right, left, top, bottom, near, far;
//             if (safe_camera->getFrustum(left, right, bottom, top, near, far)) {
//                 glm::vec2 point0 = _last_point0;
//                 _last_point0 = event.getFirstProjectPoint();

//                 glm::vec2 e = _last_point0 - point0;

//                 //计算到投影空间,逆归一化
//                 float fovy = atanf(top / near) - atanf(bottom / near);
//                 e = e / near * _distance * tanf(fovy / 2.f);
//                 e.x *= (right - left) * 2.f;
//                 e.y *= (top - bottom) * 2.f;

//                 pan(e.x, e.y, 0.f);

//                 view.needRedraw();
//                 return true;
//             }
//         }
//         return false;
//     }

//     bool Manipulator::mouseRelease(Event &event, View &view) {
//         _last_point0 = event.getFirstProjectPoint();
//         _pointer_pressed = false;
//         return false;
//     }

//     bool Manipulator::mouseMultiMove(Event &event, View &view) {
//         if (!_pointer_pressed)
//             return false;

//         glm::vec2 point0 = _last_point0;
//         glm::vec2 point1 = _last_point1;

//         _last_point0 = event.getFirstProjectPoint();
//         _last_point1 = event.getSecondProjectPoint();

//         CameraPtr safe_camera = _camera.lock();
//         if (!safe_camera) return false;

//         glm::vec2 e0 = _last_point0 - point0;
//         glm::vec2 e1 = _last_point1 - point1;

//         glm::vec2 we0, we1;
//         safe_camera->getViewport().length_projectToWindow(e0, we0);
//         safe_camera->getViewport().length_projectToWindow(e1, we1);

//         float dist_e0 = glm::length(we0);
//         float dist_e1 = glm::length(we1);
//         if ((dist_e0 <= 1.f) && (dist_e1 <= 1.f)) {
//             return false;
//         }

//         float max_dist = dist_e0;
//         float min_dist = dist_e1;

//         if (dist_e0 < dist_e1) {
//             min_dist = dist_e0;
//             max_dist = dist_e1;
//         }

//         bool ret = false;
//         if ((min_dist < 2.f) && (max_dist > min_dist * 1.5f)) {//旋转
//             glm::vec2 mid = (_last_point0 + _last_point1) / 2.f;
//             if (dist_e0 > dist_e1)
//                 ret = rotateTrackball(_last_point0 - mid, point0 - mid);
//             else
//                 ret = rotateTrackball(_last_point1 - mid, point1 - mid);
//         } else {
//             float prev_dt = glm::distance(point0, point1);
//             float last_dt = glm::distance(_last_point0, _last_point1);
//             float dt = last_dt - prev_dt;
//             if (fabsf(dt) > 0.01f) {//缩放
//                 zoom(dt > 0.f ? -_wheelZoomFactor : _wheelZoomFactor);
//                 ret = true;
//             }

//             //平移
//             float right, left, top, bottom, near, far;
//             if (safe_camera->getFrustum(left, right, bottom, top, near, far)) {
//                 glm::vec2 e = (e0 + e1) / 2.f;
//                 //计算到投影空间,逆归一化
//                 float fovy = atanf(top / near) - atanf(bottom / near);
//                 e = e / near * _distance * tanf(fovy / 2.f);
//                 e.x *= (right - left) * 2.f;
//                 e.y *= (top - bottom) * 2.f;

//                 pan(e.x, e.y, 0.f);

//                 ret = true;
//             }
//         }

//         if (ret) view.needRedraw();
//         return ret;
//     }

//     bool Manipulator::mouseScroll(Event &event, View &view) {
//         _last_point0 = event.getFirstProjectPoint();
//         _last_point1 = event.getSecondProjectPoint();
//         zoom((event.getType() == Event::POINTER_SCROLL_UP || event.getType() == Event::POINTER_SCROLL_LEFT)
//              ? -_wheelZoomFactor : _wheelZoomFactor);

//         view.needRedraw();
//         return true;
//     }

//     bool Manipulator::mouseMultiPress(Event &event, View &view) {
//         _last_point0 = event.getFirstProjectPoint();
//         _last_point1 = event.getSecondProjectPoint();
//         _pointer_pressed = true;
//         return false;
//     }

//     bool Manipulator::mouseMultiRelease(Event &event, View &view) {
//         _last_point0 = event.getFirstProjectPoint();
//         _last_point1 = event.getSecondProjectPoint();
//         _pointer_pressed = false;
//         return false;
//     }

//     Manipulator::Viewpoint Manipulator::createViewpoint(BoundingSphere &sphere) {
//         if (sphere.valid()) {
//             CameraPtr camera = _camera.lock();
//             if (camera) {
//                 float dist = sphere.radius();

//                 float left, right, bottom, top, zNear, zFar;
//                 camera->getFrustum(left, right, bottom, top, zNear, zFar);

//                 double vertical2 = fabs(right - left) / zNear / 2.0;
//                 double horizontal2 = fabs(top - bottom) / zNear / 2.0;
//                 double dim = horizontal2 < vertical2 ? horizontal2 : vertical2;
//                 double viewAngle = atan2(dim, 1.0);
//                 if (fabs(viewAngle) > 0.00000001)
//                     dist /= sin(viewAngle);

//                 glm::quat quat;
//                 return Viewpoint(sphere.center(), dist, quat);
//             }
//         }

//         return Viewpoint();
//     }

//     Manipulator::Viewpoint Manipulator::getViewpoint() {
//         glm::quat rot = glm::inverse(_rotation);
//         return Viewpoint(_center, _distance, rot);
//     }

//     void Manipulator::setViewpoint(Viewpoint &viewpoint, float duration_s) {
//         if (!viewpoint.valid())
//             return;

//         if (duration_s > 0.000001f) {
//             _flightParams._start_viewpoint = getViewpoint();
//             _flightParams._end_viewpoint = viewpoint;
//             _flightParams._start_offset = _offset;
//             _flightParams._duration_s = duration_s;
//             _flightParams._time_s = Timer::instance()->time_s();
//         } else {
//             setCenter(viewpoint.getFocalPoint());
//             setDistance(viewpoint.getRange());

//             glm::vec3 offset;
//             setOffset(offset);

//             glm::quat quat(glm::inverse(viewpoint.getQuat()));
//             setRotation(quat);
//         }
//     }

//     void Manipulator::fly(double time_s) {
//         if (!_flightParams.valid())
//             return;

//         double t = (time_s - _flightParams._time_s) / _flightParams._duration_s;
//         double tp = t;

//         if (t >= 1.0) {
//             _flightParams._duration_s = 0.0;
//         } else {
//             tp = accelerationInterp(tp, 0.4);

//             // the more smoothsteps you do, the more pronounced the fade-in/out effect
//             tp = smoothStepInterp(tp);
//             tp = smoothStepInterp(tp);
//         }

//         Viewpoint new_vp = _flightParams._start_viewpoint;
//         new_vp.slerp(tp, _flightParams._end_viewpoint);

//         setViewpoint(new_vp);

//         glm::vec3 offset = _flightParams._start_offset * (1.0 - tp);
//         _offset = offset;
//     }
// }
