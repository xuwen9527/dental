#ifndef __UI_UNDERCUT_H__
#define __UI_UNDERCUT_H__

#include <array>
#include <glm/vec2.hpp>
#include <ui/view.h>
#include <viewer.h>
#include <geometry.h>

namespace Dental::UI {
  class UnderCut : public View {
  public:
    UnderCut(Engine& engine, const std::string& name = "UnderCut", bool visible = false);

    ~UnderCut() override;

    UnderCut& operator = (UnderCut&&) noexcept = delete;
    UnderCut& operator = (const UnderCut&) = delete;
    UnderCut(const UnderCut&) = delete;
    UnderCut(UnderCut&&) noexcept = delete;

    void render() override;

    void geometry(const GeometryPtr&);

  private:
    std::array<Viewer, 3> viewers_;

    GeometryPtr geometry_;
  };

  using UnderCutPtr = std::shared_ptr<UnderCut>;
}
#endif