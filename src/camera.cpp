#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/matrix_query.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <camera.h>

namespace Dental {
  Camera::Camera() :
    Node(),
    projection_(glm::identity<glm::mat4>()) {

  }

  Camera::~Camera() {

  }

  Camera& Camera::operator = (const Camera& rhs) {
    if (this != &rhs) {
      (Node&)(*this) = (Node&)rhs;
      projection_ = rhs.projection_;
      viewport_ = rhs.viewport_;
    }
    return *this;
  }

  Camera& Camera::operator = (Camera&& rhs) noexcept {
    if (this != &rhs) {
      (Node&)(*this) = std::move((Node&)rhs);
      projection_ = std::move(rhs.projection_);
      viewport_ = std::move(rhs.viewport_);
    }
    return *this;
  }

  Camera::Camera(const Camera& rhs) {
    *this = rhs;
  }

  Camera::Camera(Camera&& rhs) noexcept {
    *this = std::move(rhs);
  }

  void Camera::accept(Visitor& visitor) {
    visitor.apply(*this);
  }

  void Camera::viewport(int x, int y, int width, int height) {
    if (orthogonal()) {
      orthoViewport(x, y, width, height);
    }
    else {
      perspectiveViewport(x, y, width, height);
    }
  }

  void Camera::home() {
    float fovy, aspect, zNear, zFar;
    if (splitPerspective(fovy, aspect, zNear, zFar)) {
      float half_height = viewport_.height() / 2.f;
      float half_width = viewport_.width() / 2.f;
      float distance = half_height / std::tan(glm::radians(fovy / 2.f));
      origin({ 0.f, 0.f, -distance });
    }
    else {
      origin({ 0.f , 0.f, 0.f });
    }
  }

  void Camera::makeViewport(int x, int y, int width, int height) {
    if (width > 2 || height > 2) {
      viewport_.set(x, y, width, height);
    }
  }

  void Camera::makeFrustum(float left, float right, float bottom, float top, float near, float far) {
    projection_ = glm::frustum(left, right, bottom, top, near, far);
  }

  bool Camera::splitFrustum(float& left, float& right, float& bottom, float& top, float& near, float& far) const {
    if (projection_[0][3] != 0.0 || projection_[1][3] != 0.0 || projection_[2][3] != -1.0 ||
      projection_[3][3] != 0.0)
      return false;

    // note: near and far must be used inside this method instead of zNear and zFar
    // because zNear and zFar are references and they may point to the same variable.
    float temp_near = projection_[3][2] / (projection_[2][2] - 1.0f);
    float temp_far = projection_[3][2] / (1.0f + projection_[2][2]);

    left = temp_near * (projection_[2][0] - 1.0f) / projection_[0][0];
    right = temp_near * (1.0f + projection_[2][0]) / projection_[0][0];

    top = temp_near * (1.0f + projection_[2][1]) / projection_[1][1];
    bottom = temp_near * (projection_[2][1] - 1.0f) / projection_[1][1];

    near = temp_near;
    far = temp_far;

    return true;
  }

  void Camera::makeOrtho(float left, float right, float bottom, float top) {
    projection_ = glm::ortho(left, right, bottom, top, -1000.f, 1000.f);
  }

  void Camera::makePerspective(float fovy, float aspect, float near, float far) {
    projection_ = glm::perspective(glm::radians(fovy), aspect, near, far);
  }

  bool Camera::splitPerspective(float& fovy, float& aspect, float& zNear, float& zFar) const {
    float right = 0.0f;
    float left = 0.0f;
    float top = 0.0f;
    float bottom = 0.0f;

    // note: near and far must be used inside this method instead of zNear and zFar
    // because zNear and zFar are references and they may point to the same variable.
    float temp_near = 0.0f;
    float temp_far = 0.0f;

    // get frustum and compute results
    bool r = splitFrustum(left, right, bottom, top, temp_near, temp_far);
    if (r) {
      fovy = atanf(top / temp_near) - atanf(bottom / temp_near);
      fovy = glm::degrees(fovy);
      aspect = (right - left) / (top - bottom);
    }
    zNear = temp_near;
    zFar = temp_far;
    return r;
  }

  bool Camera::orthogonal() const {
    return glm::isOrthogonal(projection_, 0.00001f);
  }

  void Camera::orthoViewport(int x, int y, int width, int height) {
    makeViewport(x, y, width, height);
    makeOrtho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f);
  }

  void Camera::perspectiveViewport(int x, int y, int width, int height) {
    perspectiveViewport(x, y, width, height, 90.f, 8.0f, 1000.0f);
  }

  void Camera::perspectiveViewport(int x, int y, int width, int height, float fovy, float near, float far) {
    makeViewport(x, y, width, height);
    makePerspective(fovy, (float)width / height, near, far);
  }

  void Camera::perspectiveViewport(int x, int y, int width, int height, float aspect, float fovy, float near, float far) {
    makeViewport(x, y, width, height);
    makePerspective(fovy, aspect, near, far);
  }

  glm::vec4 Camera::project(const glm::vec4& vec) const {
    glm::vec4 v = projection_ * vec;
    v = v / v.w;
    return v;
  }

  glm::vec3 Camera::project(const glm::vec3& vec) const {
    glm::vec4 v = project(glm::vec4(vec, 1.0));
    return glm::vec3(v.x, v.y, v.z);
  }

  glm::vec4 Camera::unProject(const glm::vec4& v) const {
    glm::mat4 inverse_p = glm::inverse(projection_);
    glm::vec4 vp = inverse_p * v;
    vp /= vp.w;
    return vp;
  }

  glm::vec3 Camera::unProject(const glm::vec3& v) const {
    glm::vec4 vp = unProject(glm::vec4(v, 1.0));
    return glm::vec3(vp.x, vp.y, vp.z);
  }
}