#ifndef __UI_MENUBARUI_H__
#define __UI_MENUBARUI_H__

#include <ui/view.h>

namespace Dental::UI {
  class MenuBar : public View {
  public:
    MenuBar(Engine& engine, const std::string& name = "MenuBar", bool visible = true);

    MenuBar& operator = (MenuBar&&) noexcept = delete;
    MenuBar& operator = (const MenuBar&) = delete;
    MenuBar(const MenuBar&) = delete;
    MenuBar(MenuBar&&) noexcept = delete;

    void render() override;
  };

  using MenuBarPtr = std::shared_ptr<MenuBar>;
}

#endif