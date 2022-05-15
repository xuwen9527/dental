#ifndef __UI_PROJECT_H__
#define __UI_PROJECT_H__

#include <ui/view.h>

namespace Dental::UI {
  class Project : public View {
  public:
    Project(Engine& engine, const std::string& name = "Project", bool visible = true);

    Project& operator = (Project&&) noexcept = delete;
    Project& operator = (const Project&) = delete;
    Project(const Project&) = delete;
    Project(Project&&) noexcept = delete;

    void render() override;
  };

  using ProjectPtr = std::shared_ptr<Project>;
}

#endif