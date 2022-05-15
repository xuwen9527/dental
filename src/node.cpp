#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <node.h>
#include <uuid.h>

namespace Dental {
  Node::Node() :
    mv_(glm::identity<glm::mat4>()),
    uuid_(createUUID()) {

  }

  Node::~Node() {

  }

  Node& Node::operator = (const Node& rhs) {
    if (this != &rhs) {
      name_ = rhs.name_;
      mv_ = rhs.mv_;
	    uuid_ = rhs.uuid_;
      geometrys_ = rhs.geometrys_;
    }
    return *this;
  }

  Node& Node::operator = (Node&& rhs) noexcept {
    if (this != &rhs) {
      name_ = std::move(name_);
      mv_ = std::move(rhs.mv_);
	    uuid_ = std::move(rhs.uuid_);
      geometrys_ = std::move(rhs.geometrys_);
    }
    return *this;
  }

  Node::Node(const Node& rhs) {
    *this = rhs;
  }

  Node::Node(Node&& rhs) noexcept {
    *this = std::move(rhs);
  }

  glm::vec3 Node::origin() const {
    return glm::vec3(mv_[3][0], mv_[3][1], mv_[3][2]);
  }

  void Node::origin(const glm::vec3& origin) {
    mv_[3].x = origin.x;
    mv_[3].y = origin.y;
    mv_[3].z = origin.z;
  }

  void Node::get_scale_rotate_translate(glm::vec3& scale, glm::quat& rotate, glm::vec3& translate) const {
    translate = mv_[3];
    for (int i = 0; i < 3; i++) {
      scale[i] = glm::length(glm::vec3(mv_[i]));
    }

    const glm::mat3 rotate_matrix(
      glm::vec3(mv_[0]) / scale[0],
      glm::vec3(mv_[1]) / scale[1],
      glm::vec3(mv_[2]) / scale[2]);

    rotate = glm::quat_cast(rotate_matrix);
  }

  void Node::set_scale_rotate_translate(glm::vec3 scale, glm::quat rotate, glm::vec3 translate) {
    mv_ = glm::translate(translate) * glm::mat4_cast(rotate) * glm::scale(scale);
  }

  void Node::translate(const glm::vec3& trans) {
    mv_ = glm::translate(mv_, trans);
  }

  void Node::rotate(float angle, const glm::vec3& normal) {
    mv_ = glm::rotate(mv_, angle, normal);
  }

  void Node::scale(const glm::vec3& scale) {
    mv_ = glm::scale(mv_, scale);
  }

  void Node::addGeometry(const GeometryPtr& geometry) {
    auto itr = std::find(geometrys_.begin(), geometrys_.end(), geometry);
    if (itr == geometrys_.end()) {
      geometry->parent(ptr());
      geometrys_.emplace_back(geometry);
    }
  }

  int Node::geometryIndex(const GeometryPtr& geometry) {
    auto itr = std::find(geometrys_.begin(), geometrys_.end(), geometry);
    if (itr != geometrys_.end()) {
      return (int)(itr - geometrys_.begin());
    }
    return -1;
  }

  void Node::insertGeometry(unsigned int index, const GeometryPtr& geometry) {
    if (index < geometrys_.size()) {
      auto pos_geometry = geometrys_[index];
      auto itr = std::find(geometrys_.begin(), geometrys_.end(), pos_geometry);
      if (itr != geometrys_.end()) {
        geometry->parent(ptr());
        geometrys_.emplace(itr, geometry);
      }
    }
  }

  GeometryPtr Node::geometry(unsigned int index) const {
    if (index >= geometrys_.size()) {
      return nullptr;
    }

    return geometrys_[index];
  }

  unsigned int Node::numGeometry() const {
    return geometrys_.size();
  }

  GeometryPtr Node::geometry(const std::string& name) const {
    for (auto geometry : geometrys_) {
      if (geometry->name() == name) {
        return geometry;
      }
    }
    return nullptr;
  }

  bool Node::removeGeometry(unsigned int index) {
    if (index >= geometrys_.size()) {
      return false;
    }
    geometrys_[index]->parent(NodePtr());
    geometrys_.erase(geometrys_.begin() + index);
    return true;
  }

  bool Node::removeGeometry(const GeometryPtr& geometry) {
    auto itr = std::find(geometrys_.begin(), geometrys_.end(), geometry);
    if (itr != geometrys_.end()) {
      geometry->parent(NodePtr());
      geometrys_.erase(itr);
      return true;
    }
    return false;
  }

  void Node::clearGeometry() {
    for (auto& geometry : geometrys_) {
      geometry->parent(NodePtr());
    }
    decltype(geometrys_)().swap(geometrys_);
  }

  void Node::accept(Visitor& visitor) {
    visitor.apply(*this);
  }

  void Node::traverse(Visitor& visitor) {
    for (auto& geometry : geometrys_) {
      geometry->accept(visitor);
    }
  }
}