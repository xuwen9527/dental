#ifndef __RENDER_VISITOR_H__
#define __RENDER_VISITOR_H__

#include <visitor.h>
#include <render_info.h>

namespace Dental {
  class RenderVisitor : public Visitor {
  public:
    RenderVisitor(RenderInfoPtr& render_info);
    ~RenderVisitor() override;

    RenderVisitor& operator = (RenderVisitor&&) noexcept = delete;
    RenderVisitor& operator = (const RenderVisitor&) = delete;
    RenderVisitor(const RenderVisitor&) = delete;
    RenderVisitor(RenderVisitor&&) noexcept = delete;

    virtual Type type() override { return Type::RENDER_VISITOR; }

    virtual void apply(Geometry& geometry) override;

  protected:
    void pushProjection(const glm::mat4& projection) override;
    void popProjection() override;

    void pushViewport(const Viewport& viewport) override;
    void popViewport() override;

    RenderInfoPtr render_info_;
  };

  using RenderVisitorPtr = std::shared_ptr<RenderVisitor>;
}
#endif