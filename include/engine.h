#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <memory>
#include <ui/view.h>
#include <viewer.h>

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

    ViewerPtr& viewer() { return viewer_; }

  protected:
    void setup();
    void setupFonts();

  private:
    GLFWwindow* window_;

    UI::ViewPtrs uiviews_;

    ViewerPtr viewer_;
  };

  using EnginePtr = std::shared_ptr<Engine>;
}

#endif