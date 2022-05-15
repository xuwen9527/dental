#ifndef __UI_VIEW_H__
#define __UI_VIEW_H__

#include <string>
#include <memory>
#include <vector>

namespace Dental {
  class Engine;

  namespace UI {
    class View {
    public:
      View(Engine& engine, const std::string& name = "View", bool visible = true) :
        Name(name),
        Visible(visible),
        engine_(engine) {

      }

      virtual ~View() {}

      View& operator = (View&&) noexcept = delete;
      View& operator = (const View&) = delete;
      View(const View&) = delete;
      View(View&&) noexcept = delete;

      virtual void render() = 0;

      std::string Name;
      bool Visible;

    protected:
      Engine& engine_;
    };

    using ViewPtr = std::shared_ptr<View>;
    using ViewPtrs = std::vector<ViewPtr>;
  }
}

#endif