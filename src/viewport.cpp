#include <glad/glad.h>
#include <viewport.h>

namespace Dental {
  Viewport::Viewport() : x_(0), y_(0), width_(600), height_(400) {
  }

  Viewport::Viewport(int x, int y, int width, int height) :
    x_(x), y_(y), width_(width), height_(height) {
  }

  Viewport& Viewport::operator = (const Viewport& rhs) {
    if (this != &rhs) {
      x_ = rhs.x_;
      y_ = rhs.y_;
      width_ = rhs.width_;
      height_ = rhs.height_;
    }
    return *this;
  }

  Viewport& Viewport::operator = (Viewport&& rhs) noexcept {
    if (this != &rhs) {
      x_ = std::move(rhs.x_);
      y_ = std::move(rhs.y_);
      width_ = std::move(rhs.width_);
      height_ = std::move(rhs.height_);
    }
    return *this;
  }

  Viewport::Viewport(const Viewport& rhs) {
    *this = rhs;
  }

  Viewport::Viewport(Viewport&& rhs) noexcept {
    *this = std::move(rhs);
  }

  void Viewport::set(int x, int y, int width, int height) {
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
  }

  void Viewport::windowToProject(glm::vec2& project_point, const glm::vec2& window_point)  const {
    windowToProject(project_point.x, project_point.y, window_point.x, window_point.y);
  }

  void Viewport::projectToWindow(const glm::vec2& project_point, glm::vec2& window_point)  const {
    projectToWindow(project_point.x, project_point.y, window_point.x, window_point.y);
  }

  void Viewport::windowToProject(float &px, float &py, float wx, float wy)  const {
    if (width_ < 2 || height_ < 2) {
      px = wx / width_;
      py = wy / height_;
      return;
    }

    px = (wx - x_) / width_ * 2.f - 1.f;
    py = (height_ - y_ - wy) / height_ * 2.f - 1.f;
  }

  void Viewport::projectToWindow(float px, float py, float& wx, float& wy)  const {
    if (width_ < 2 || height_ < 2) {
      wx = px * width_;
      wy = py * height_;
      return;
    }

    wx = (px + 1.f) / 2.f * width_ + x_;
    wy = height_ - y_ - (py + 1.f) / 2.f * height_;
  }

  void Viewport::lengthWindowToProject(glm::vec2& project_point, const glm::vec2& window_point) const {
    lengthWindowToProject(project_point.x, project_point.y, window_point.x, window_point.y);
  }

  void Viewport::lengthProjectToWindow(const glm::vec2& project_point, glm::vec2& window_point) const {
    lengthProjectToWindow(project_point.x, project_point.y, window_point.x, window_point.y);
  }

  void Viewport::lengthWindowToProject(float &px, float &py, float wx, float wy) const {
    px = wx / width_ * 2.f;
    py = wy / height_ * 2.f;
  }

  void Viewport::lengthProjectToWindow(float px, float py, float& wx, float& wy) const {
    wx = px * width_ * 0.5f;
    wy = py * height_ * 0.5f;
  }

  void Viewport::windowToViewport(glm::vec2& viewport_point, const glm::vec2& window_point) const {
    windowToViewport(viewport_point.x, viewport_point.y, window_point.x, window_point.y);
  }

  void Viewport::viewportToWindow(const glm::vec2& viewport_point, glm::vec2& window_point) const {
    viewportToWindow(viewport_point.x, viewport_point.y, window_point.x, window_point.y);
  }

  void Viewport::windowToViewport(float &vx, float &vy, float wx, float wy) const {
    vx = (wx - x_) - width_ / 2.f;
    vy = height_ / 2.f - (wy - y_);
  }

  void Viewport::viewportToWindow(float vx, float vy, float& wx, float& wy) const {
    wx =  vx + width_ / 2.f + x_;
    wy = -vy + height_ / 2.f + y_; 
  }

  void Viewport::apply() const {
    glViewport(x_, y_, width_, height_);
  }
}