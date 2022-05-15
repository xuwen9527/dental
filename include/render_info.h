#ifndef __RENDER_INFO_H__
#define __RENDER_INFO_H__

#include <memory>
#include <glm/mat4x4.hpp>
#include <viewport.h>

namespace Dental {
  class RenderInfo {
  public:
    RenderInfo();
    ~RenderInfo();

    RenderInfo& operator = (const RenderInfo& rhs);
    RenderInfo& operator = (RenderInfo&& rhs) noexcept;
    RenderInfo(const RenderInfo& rhs);
    RenderInfo(RenderInfo&& rhs) noexcept;

    void mv(const glm::mat4& mv);
    const glm::mat4& mv() const;

    void projection(const glm::mat4& projection);
    const glm::mat4& projection() const;

    void mvp(const glm::mat4& mv, const glm::mat4& projection);
    glm::mat4 mvp();

    void viewport(const Viewport& viewport);
    const Viewport& viewport() const;

  protected:
    glm::mat4 mv_;
    glm::mat4 projection_;
    Viewport viewport_;
  };

  using RenderInfoPtr = std::shared_ptr<RenderInfo>;
}
#endif