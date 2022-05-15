#include <glm/gtc/matrix_transform.hpp>
#include <render_info.h>

namespace Dental {
  RenderInfo::RenderInfo() : 
    projection_(glm::identity<glm::mat4>()) {
  }

  RenderInfo::~RenderInfo() {

  }

  RenderInfo& RenderInfo::operator = (const RenderInfo& rhs) {
    if (this != &rhs) {
      mv_ = rhs.mv_;
      projection_ = rhs.projection_;
      viewport_ = rhs.viewport_;
    }
    return *this;
  }

  RenderInfo& RenderInfo::operator = (RenderInfo&& rhs) noexcept {
    if (this != &rhs) {
      mv_ = std::move(rhs.mv_);
      projection_ = std::move(rhs.projection_);
      viewport_ = std::move(rhs.viewport_);
    }
    return *this;
  }

  RenderInfo::RenderInfo(const RenderInfo& rhs) {
    *this = rhs;
  }

  RenderInfo::RenderInfo(RenderInfo&& rhs) noexcept {
    *this = std::move(rhs);
  }

  void RenderInfo::mv(const glm::mat4& mv) {
    mv_ = mv;
  }

  const glm::mat4& RenderInfo::mv() const {
    return mv_;
  }

  void RenderInfo::projection(const glm::mat4& projection) {
    projection_ = projection;
  }

  const glm::mat4& RenderInfo::projection() const {
    return projection_;
  }

  void RenderInfo::mvp(const glm::mat4& mv, const glm::mat4& projection) {
    mv_ = mv;
    projection_ = projection;
  }

  glm::mat4 RenderInfo::mvp() {
    return projection_ * mv_; 
  }

  void RenderInfo::viewport(const Viewport& viewport) {
    viewport_ = viewport;
  }

  const Viewport& RenderInfo::viewport() const {
    return viewport_;
  }
}
