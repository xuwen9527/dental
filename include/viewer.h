#ifndef __VIEWER_H__
#define __VIEWER_H__

#include <memory>
#include <scene.h>
#include <manipulator.h>
#include <events.h>

namespace Dental {
  class Viewer : public std::enable_shared_from_this<Viewer> {
  public:
    Viewer();
    ~Viewer();

    Viewer& operator = (Viewer&&) noexcept = delete;
    Viewer& operator = (const Viewer&) = delete;
    Viewer(const Viewer&) = delete;
    Viewer(Viewer&&) noexcept = delete;

    void frame();

    void home(float duration_s = 0.0f);

    ScenePtr& scene() { return scene_; }

    Events& events() { return events_; }

  protected:

    void render(RenderInfoPtr& render_info);

    bool handleEvent(Event &event);

    ScenePtr scene_;
    
    ManipulatorPtr manipulator_;

    Events events_;
  };

  using ViewerPtr = std::shared_ptr<Viewer>;
}

#endif