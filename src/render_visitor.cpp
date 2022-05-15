#include <render_visitor.h>
#include <geometry.h>
#include <node.h>

namespace Dental {
  RenderVisitor::RenderVisitor(RenderInfoPtr& render_info) :
    Visitor(),
    render_info_(render_info) {
  }

  RenderVisitor::~RenderVisitor() {
  }

  void RenderVisitor::pushProjection(const glm::mat4& projection) {
    Visitor::pushProjection(projection);
    render_info_->projection(projection);
  }

  void RenderVisitor::popProjection() {
    Visitor::popProjection();
    if (!projections_.empty()) {
      render_info_->projection(projections_.top());
    }
  }

  void RenderVisitor::pushViewport(const Viewport& viewport) {
    Visitor::pushViewport(viewport);
    render_info_->viewport(viewport);
    viewport.apply();
  }

  void RenderVisitor::popViewport() {
    Visitor::popViewport();
    if (!viewports_.empty()) {
      auto viewport = viewports_.top();
      render_info_->viewport(viewport);
      viewport.apply();
    }
  }

  void RenderVisitor::apply(Geometry& geometry) {
    pushMV(geometry.mv());
    render_info_->mv(mvs_.top());
    geometry.render(*render_info_);
    popMV();
  }
}