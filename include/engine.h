#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <memory>
#include <ui/view.h>
#include <scene.h>
#include <manipulator.h>

struct GLFWwindow;

namespace Dental {
  class Engine : public std::enable_shared_from_this<Engine> {
  public:
    Engine();
    ~Engine();

    Engine& operator = (Engine&&) noexcept = delete;
    Engine& operator = (const Engine&) = delete;
    Engine(const Engine&) = delete;
    Engine(Engine&&) noexcept = delete;

    inline GLFWwindow* window() { return window_; }
    inline UI::ViewPtrs& uiviews() { return uiviews_; }

    void run();

    void frame();

    void home(float duration_s = 0.0f);

    ScenePtr& scene() {
      return scene_;
    }

  protected:
    void setup();
    void setupFonts();

  private:
    GLFWwindow* window_;

    ScenePtr scene_;
    
    ManipulatorPtr manipulator_;

    UI::ViewPtrs uiviews_;
  };

  using EnginePtr = std::shared_ptr<Engine>;
}

#endif