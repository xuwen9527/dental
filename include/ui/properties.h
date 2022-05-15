#ifndef __UI_PROPERTIES_H__
#define __UI_PROPERTIES_H__

#include <ui/view.hxx>

namespace Dental::UI {
  class Properties : public View {
  public:
    Properties(Engine& engine, const std::string& name = "Properties", bool visible = true);

    Properties& operator = (Properties&&) noexcept = delete;
    Properties& operator = (const Properties&) = delete;
    Properties(const Properties&) = delete;
    Properties(Properties&&) noexcept = delete;

    void render() override;
  };

  using PropertiesPtr = std::shared_ptr<Properties>;
}

#endif