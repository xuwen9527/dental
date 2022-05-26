#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <node.h>
#include <viewport.h>

namespace Dental {
  class Camera : public Node {
  public:
    Camera();
    virtual ~Camera() override;

    Mate_Node(Dental, Camera)

    Camera& operator = (const Camera&);
    Camera& operator = (Camera&&) noexcept;
    Camera(const Camera&);
    Camera(Camera&&) noexcept;

    inline virtual void accept(Visitor& visitor) override;

    inline void projection(const glm::mat4& projection) { projection_ = projection; }
    inline const glm::mat4& projection() const { return projection_; }

    void viewport(int x, int y, int width, int height);
    inline const Viewport& viewport() const { return viewport_; }

    inline void resize(int width, int height) {
      viewport(0, 0, width, height);
    }

    bool splitPerspective(float& fovy, float& aspect, float& near, float& far) const;
    bool splitFrustum(float& left, float& right, float& bottom, float& top, float& near, float& far) const;
    void perspectiveViewport(int x, int y, int width, int height, float fovy, float near, float far);
    void perspectiveViewport(int x, int y, int width, int height, float aspect, float fovy, float near, float far);

    void home();

    glm::vec4 project(const glm::vec4& v) const;
    glm::vec3 project(const glm::vec3& v) const;

    glm::vec3 unProject(const glm::vec3& v) const;
    glm::vec4 unProject(const glm::vec4& v) const;

    bool orthogonal() const;

    void orthoViewport(int x, int y, int width, int height);
    void perspectiveViewport(int x, int y, int width, int height);

  protected:
    void makeViewport(int x, int y, int width, int height);

    void makeFrustum(float left, float right, float bottom, float top, float near, float far);

    void makeOrtho(float left, float right, float bottom, float top);

    void makePerspective(float fovy, float aspect, float near, float far);

    glm::mat4 projection_;

    Viewport viewport_;
  };

  using CameraPtr = std::shared_ptr<Camera>;
}
#endif