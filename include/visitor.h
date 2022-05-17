#ifndef __VISITOR_H__
#define __VISITOR_H__

#include <memory>
#include <viewport.h>
#include <matrix_cascade.h>
#include <bounding_sphere.h>

namespace Dental {
  class Scene;
  class Camera;
  class Node;
  class Geometry;

  class Visitor {
  public:
    enum class Type {
      DEFAULT,
      RENDER_VISITOR
    };

    Visitor();
    virtual ~Visitor();

    Visitor& operator = (Visitor&&) noexcept = delete;
    Visitor& operator = (const Visitor&) = delete;
    Visitor(const Visitor&) = delete;
    Visitor(Visitor&&) noexcept = delete;

    virtual Type type() { return Type::DEFAULT; }

    virtual void apply(Node& node);

    virtual void apply(Camera& camera);

    inline virtual void apply(Scene& scene) {
      apply((Camera&)scene);
    }

    virtual void apply(Geometry&) {}

  protected:
    virtual void pushMV(const glm::mat4&);
    virtual void popMV();

    virtual void pushProjection(const glm::mat4&);
    virtual void popProjection();

    virtual void pushViewport(const Viewport&);
    virtual void popViewport();

    MatrixCascade mvs_;
    std::stack<glm::mat4> projections_;
    std::stack<Viewport> viewports_;
  };

  using VisitorPtr = std::shared_ptr<Visitor>;

  class ComputeBoundingSphereVisitor : public Visitor {
    public:
      ComputeBoundingSphereVisitor();

      virtual void apply(Geometry& geometry) override;

      BoundingSphere& boundingSphere() { return boundingSphere_; }

    protected:
      BoundingSphere boundingSphere_;
    };
    using ComputeBoundingSphereVisitorPtr = std::shared_ptr<ComputeBoundingSphereVisitor>;
}
#endif