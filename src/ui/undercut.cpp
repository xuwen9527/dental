#include "../external/imgui/imgui.h"
#include <ui/undercut.h>
#include <engine.h>

namespace Dental::UI {
  UnderCut::UnderCut(Engine& engine, const std::string& name, bool visible) :
    View(engine, name, visible) {
  }

  UnderCut::~UnderCut() {
  }

  void UnderCut::geometry(const GeometryPtr& geometry) {
    geometry_ = geometry;
    for (auto& viewer : viewers_) {
      viewer.scene()->clearGeometry();
      viewer.scene()->addGeometry(geometry);
    }
  }

  void UnderCut::render() {
    if (!Visible) {
      return;
    }

    if (ImGui::Begin(Name.c_str(), &Visible)) {
      ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
      ImGui::Button("设置就位道");
      if (ImGui::BeginTable("##", 2, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableNextRow();
        {
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("Angle");

          static float angle = 0.0f;

          ImGui::TableSetColumnIndex(1);
          ImGui::SetNextItemWidth(-FLT_MIN);
          if (ImGui::InputFloat("##", &angle)) {
          }
        }

        ImGui::EndTable();
      }
      ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
      ImGui::NewLine();
    }
    ImGui::End();

    if (!geometry_) {
      geometry(engine_.viewer()->scene()->geometry(0));
    }

    if (geometry_) {
      auto& viewport = engine_.viewer()->scene()->viewport();
      auto width = viewport.width() / 3;
      auto height = viewport.height() / 4;

      glm::quat quat(glm::identity<glm::quat>());
      quat = glm::rotate(quat, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
      quat = glm::rotate(quat, glm::radians(-180.f), glm::vec3(0.f, 0.f, 1.f));

      for (int i = 0; i < viewers_.size(); ++i) {
        auto& viewer = viewers_[i];
        quat = glm::rotate(quat, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
        viewer.scene()->viewport(width * i, 0, width, height);
        viewer.home(0, quat, 0.5f);
        viewer.frame();
      }
    }
  }
}