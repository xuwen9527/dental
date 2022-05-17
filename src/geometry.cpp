#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <geometry.h>
#include <uuid.h>

namespace Dental {
  Geometry::Geometry() :
    mv_(glm::identity<glm::mat4>()),
    dirty_(true),
    vertex_array_(std::make_shared<Vec3Array>()),
    normal_array_(std::make_shared<Vec3Array>()),
    color_array_(std::make_shared<Vec4Array>()),
    texcoord_array_(std::make_shared<Vec2Array>()),
    uuid_(createUUID()),
    dirty_bounding_(true),
    render_technique_(std::make_shared<DefaultRenderTechnique>()) {
    vertex_array_->bind(static_cast<std::underlying_type<Attrib>::type>(Attrib::POSITION));
    normal_array_->bind(static_cast<std::underlying_type<Attrib>::type>(Attrib::NORMAL));
    color_array_->bind(static_cast<std::underlying_type<Attrib>::type>(Attrib::COLOR));
    texcoord_array_->bind(static_cast<std::underlying_type<Attrib>::type>(Attrib::TEXCOORD));
  }

  Geometry::~Geometry() {

  }

  Geometry& Geometry::operator = (const Geometry& rhs) {
    if (this != &rhs) {
      parent_ = rhs.parent_;
      name_ = rhs.name_;
      mv_ = rhs.mv_;
      *vertex_array_ = *rhs.vertex_array_;
      *normal_array_ = *rhs.normal_array_;
      *color_array_ = *rhs.color_array_;
      *texcoord_array_ = *rhs.texcoord_array_;
	    uuid_ = rhs.uuid_;
      dirty_bounding_ = rhs.dirty_bounding_;
      bounding_sphere_ = rhs.bounding_sphere_;

      decltype(primitive_sets_)().swap(primitive_sets_);
      for (const auto& primitive_set : rhs.primitive_sets_) {
        primitive_sets_.emplace_back(primitive_set->clone());
      }
      
      decltype(textures_)().swap(textures_);
      for (const auto& itr : rhs.textures_) {
        textures_.insert({ itr.first, itr.second->clone() });
      }
      dirty();
      dirtyBounding();
    }
    return *this;
  }

  Geometry& Geometry::operator = (Geometry&& rhs) noexcept {
    if (this != &rhs) {
      parent_ = std::move(rhs.parent_);
      name_ = std::move(rhs.name_);
      mv_ = std::move(rhs.mv_);
      *vertex_array_ = std::move(*rhs.vertex_array_);
      *normal_array_ = std::move(*rhs.normal_array_);
      *color_array_ = std::move(*rhs.color_array_);
      *texcoord_array_ = std::move(*rhs.texcoord_array_);
      primitive_sets_ = std::move(rhs.primitive_sets_);
      textures_ = std::move(rhs.textures_);
	    uuid_ = std::move(rhs.uuid_);
      dirty_bounding_ = std::move(rhs.dirty_bounding_);
      bounding_sphere_ = std::move(rhs.bounding_sphere_);
      dirty();
      dirtyBounding();
    }
    return *this;
  }

  Geometry::Geometry(const Geometry& rhs) {
    *this = rhs;
  }

  Geometry::Geometry(Geometry&& rhs) noexcept {
    *this = std::move(rhs);
  }

  NodePtr Geometry::parent() const {
    if (!parent_.expired()) {
      return parent_.lock();
    }
    return nullptr;
  }

  void Geometry::accept(Visitor& visitor) {
    visitor.apply(*this);
  }

  glm::vec3 Geometry::origin() const {
    return glm::vec3(mv_[3][0], mv_[3][1], mv_[3][2]);
  }

  void Geometry::origin(glm::vec3 origin) {
    mv_[3].x = origin.x;
    mv_[3].y = origin.y;
    mv_[3].z = origin.z;
  }

  void Geometry::get_scale_rotate_translate(glm::vec3& scale, glm::quat& rotate, glm::vec3& translate) const {
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

  void Geometry::set_scale_rotate_translate(glm::vec3 scale, glm::quat rotate, glm::vec3 translate) {
    mv_ = glm::translate(translate) * glm::mat4_cast(rotate) * glm::scale(scale);
  }

  void Geometry::translate(const glm::vec3& trans) {
    mv_ = glm::translate(mv_, trans);
  }

   void Geometry::rotate(float angle, const glm::vec3& normal) {
     mv_ = glm::rotate(mv_, angle, normal);
   }

  void Geometry::angle(const glm::vec3 angle) {
    glm::vec3 translate{};
    glm::vec3 scale{};
    glm::quat rotate{};
    get_scale_rotate_translate(scale, rotate, translate);
    glm::vec3 angle_temp = glm::radians(angle);
    rotate = glm::quat_cast(glm::eulerAngleXYZ(angle_temp.x, angle_temp.y, angle_temp.z));
    set_scale_rotate_translate(scale, rotate, translate);
  }

  glm::vec3 Geometry::angle() const {
    glm::vec3 angle;
    glm::extractEulerAngleXYZ(mv_, angle.x, angle.y, angle.z);
    angle = glm::degrees(angle);    
    return angle;
  }

  void Geometry::set_scale(const glm::vec3 scale) {
    mv_ = glm::scale(mv_, scale);
  }

  void Geometry::scale(const glm::vec3 scale) {
    glm::vec3 translate{};
    glm::vec3 scale_temp{};
    glm::quat rotate{};
    get_scale_rotate_translate(scale_temp, rotate, translate);
    set_scale_rotate_translate(scale, rotate, translate);
  }

  glm::vec3 Geometry::scale() const {
    return glm::vec3(glm::length(glm::vec3(mv_[0])), glm::length(glm::vec3(mv_[1])), glm::length(glm::vec3(mv_[2])));
  }

  void Geometry::addPrimitiveSet(const PrimitiveSetPtr& primitive_set) {
    primitive_sets_.emplace_back(primitive_set);
  }

  void Geometry::setPrimitiveSet(const PrimitiveSetPtr& primitive_set) {
    primitive_sets_.clear();
    primitive_sets_.emplace_back(primitive_set);
  }

  PrimitiveSetPtr Geometry::primitiveSet(unsigned int index) const {
    if (index >= primitive_sets_.size()) {
      return nullptr;
    }
    return primitive_sets_[index];
  }

  void Geometry::removePrimitiveSet(unsigned int index) {
    if (index < primitive_sets_.size()) {
      primitive_sets_.erase(primitive_sets_.begin() + index);
    }
  }

  void Geometry::clearPrimitiveSets() {
    primitive_sets_.clear();
  }

  void Geometry::dirty() {
    dirty_ = true;
  }

  void Geometry::texture(const TexturePtr& texture, unsigned int target) {
    textures_[target] = texture;
  }

  TexturePtr Geometry::texture(unsigned int target) const {
    for (auto& itr : textures_) {
      if (itr.first == target) {
        return itr.second;
      }
    }
    return nullptr;
  }

  void Geometry::removeTexture(unsigned int target) {
    for (auto itr = textures_.begin(); itr != textures_.end(); ++itr) {
      if (itr->first == target) {
        textures_.erase(itr);
        return;
      }
    }
  }

  void Geometry::clearTexture() {
    decltype(textures_)().swap(textures_);
  }

  void Geometry::renderTechnique(const RenderTechniquePtr& render_technique) {
    render_technique_ = render_technique;
  }

  RenderTechniquePtr& Geometry::renderTechnique() {
    return render_technique_;
  }

  const RenderTechniquePtr& Geometry::renderTechnique() const {
    return render_technique_;
  }

  void Geometry::dirtyGLObjects() {
    vertex_array_->dirty();
    normal_array_->dirty();
    color_array_->dirty();
    texcoord_array_->dirty();

    for (auto& primitive_set : primitive_sets_) {
      primitive_set->dirty();
    }

    for (auto& itr : textures_) {
      itr.second->bind(itr.first);
      itr.second->dirty();
    }
  }

  void Geometry::setupGLObjects() {
    decltype(gl_objects_)().swap(gl_objects_);

    for (auto& itr : textures_) {
      gl_objects_.emplace_back(itr.second->GLObject());
    }

    gl_objects_.emplace_back(vertex_array_->GLObject());
    gl_objects_.emplace_back(normal_array_->GLObject());
    gl_objects_.emplace_back(color_array_->GLObject());
    gl_objects_.emplace_back(texcoord_array_->GLObject());

    for (auto& primitive_set : primitive_sets_) {
      gl_objects_.emplace_back(primitive_set->GLObject());
    }
  }

  BoundingBox Geometry::boundingBox() const {
    BoundingBox box;
    for (auto& vertex : *vertex_array_) {
      box.expandBy({ vertex.x, vertex.y, vertex.z });
    }
    return box;
  }

  void Geometry::render(RenderInfo& info) {
    if (render_technique_) {
      render_technique_->apply(info, *this);
    }

    if (dirty_) {
      dirtyGLObjects();
      setupGLObjects();
      dirty_ = false;
    }

    gl_objects_.render();
  }

  void Geometry::dirtyBounding() {
    dirty_bounding_ = true;
  }

  void Geometry::computeBounding() {
    if (!bounding_sphere_.valid()) {
      dirty_bounding_ = true;
    }

    if (!dirty_bounding_) {
      return;
    }

    dirty_bounding_ = false;

    bounding_sphere_.init();

    if (vertex_array_->empty()) {
      return;
    }

    BoundingBox box;
    box.expandBy(vertex_array_);

    glm::vec4 min = mv_ * glm::vec4(box.min(), 1.f);
    glm::vec4 max = mv_ * glm::vec4(box.max(), 1.f);

    bounding_sphere_.expandBy(glm::vec3(min / min.w));
    bounding_sphere_.expandBy(glm::vec3(max / max.w));
  }

  BoundingSphere &Geometry::boundingSphere() {
    if (dirty_bounding_) {
      computeBounding();
      dirty_bounding_ = false;
    }
    return bounding_sphere_;
  }
}