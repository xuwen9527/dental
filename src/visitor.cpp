#include <visitor.h>
#include <node.h>
#include <camera.h>

namespace Dental {
  Visitor::Visitor() {
  }

  Visitor::~Visitor() {
  }

  void Visitor::pushMV(const glm::mat4& mv) {
    mvs_.push(mv);
  }

  void Visitor::popMV() {
    mvs_.pop();
  }

  void Visitor::pushProjection(const glm::mat4& projection) {
    projections_.push(projection);
  }

  void Visitor::popProjection() {
    projections_.pop();
  }

  void Visitor::pushViewport(const Viewport& viewport) {
    viewports_.push(viewport);
  }

  void Visitor::popViewport() {
    viewports_.pop();
  }

  void Visitor::apply(Node& node) {
    pushMV(node.mv());
    node.traverse(*this);
    popMV();
  }

  void Visitor::apply(Camera& camera) {
    pushProjection(camera.projection());
    pushViewport(camera.viewport());

    apply((Node&)camera);

    popViewport();
    popProjection();
  }

  ComputeBoundingSphereVisitor::ComputeBoundingSphereVisitor() : Visitor() {
    boundingSphere_.init();
  }

  void ComputeBoundingSphereVisitor::apply(Geometry& geometry) {
    BoundingSphere &bound = geometry.boundingSphere();
    if (bound.valid()) boundingSphere_.expandBy(bound);
  }
}