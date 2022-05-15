#include "../external/imgui/imgui.h"
#include <engine.h>
#include <ui/project.h>
#include <config.h>

namespace Dental::UI {
  Project::Project(Engine& engine, const std::string& name, bool visible) :
    View(engine, name, visible) {

  }

  void Project::render() {
    if (!Visible) {
      return;
    }

    if (ImGui::Begin(Name.c_str(), &Visible)) {
      // if (ImGui::BeginTable("##", 2, ImGuiTableFlags_SizingStretchProp)) {

      // }
      // ImGui::EndTable();
    }
    ImGui::End();
  }
}