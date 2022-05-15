#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

#include <memory>
#include <glm/ext.hpp>

namespace Dental {
  class Viewport {
  public:
    Viewport();
    Viewport(int x, int y, int width, int height);

    Viewport& operator = (const Viewport&) ;
    Viewport& operator = (Viewport&&) noexcept ;
    Viewport(const Viewport&);
    Viewport(Viewport&&) noexcept;

    inline int x() const { return x_; }
    inline int y() const { return y_; }
    inline int width() const { return width_; }
    inline int height() const { return height_; }

    void set(int x, int y, int width, int height);

    void windowToProject(glm::vec2& project_point, const glm::vec2& window_point) const;
    void projectToWindow(const glm::vec2& project_point, glm::vec2& window_point) const;

    void windowToProject(float &px, float &py, float wx, float wy) const;
    void projectToWindow(float px, float py, float& wx, float& wy) const;

    void lengthWindowToProject(glm::vec2& project_point, const glm::vec2& window_point) const;
    void lengthProjectToWindow(const glm::vec2& project_point, glm::vec2& window_point) const;

    void lengthWindowToProject(float& px, float& py, float wx, float wy) const;
    void lengthProjectToWindow(float px, float py, float& wx, float& wy) const;

    void windowToViewport(glm::vec2& viewport_point, const glm::vec2& window_point) const;
    void viewportToWindow(const glm::vec2& viewport_point, glm::vec2& window_point) const;

    void windowToViewport(float &vx, float &vy, float wx, float wy) const;
    void viewportToWindow(float vx, float vy, float& wx, float& wy) const;

    void apply() const;

private:
    int x_;
    int y_;
    int width_;
    int height_;
  };

  using ViewportPtr = std::shared_ptr<Viewport>;
}
#endif