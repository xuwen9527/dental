#ifndef __UI_PREVIEW_H__
#define __UI_PREVIEW_H__

#include <glm/vec2.hpp>
#include <ui/view.h>

namespace Dental::UI {
  class Preview : public View {
  public:
    Preview(Engine& engine, const std::string& name = "Preview", bool visible = true);

    ~Preview() override;

    Preview& operator = (Preview&&) noexcept = delete;
    Preview& operator = (const Preview&) = delete;
    Preview(const Preview&) = delete;
    Preview(Preview&&) noexcept = delete;

    void render() override;

  protected:
    void render_to_frame_buffer(unsigned int width, unsigned int height);
  };

  using PreviewPtr = std::shared_ptr<Preview>;
}
#endif